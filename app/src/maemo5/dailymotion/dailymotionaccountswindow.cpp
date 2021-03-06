/*
 * Copyright (C) 2016 Stuart Howarth <showarth@marxoft.co.uk>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 3 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "dailymotionaccountswindow.h"
#include "accountdelegate.h"
#include "dailymotion.h"
#include "dailymotionaccountmodel.h"
#include "dailymotionauthdialog.h"
#include "listview.h"
#include <qdailymotion/authenticationrequest.h>
#include <qdailymotion/resourcesrequest.h>
#include <QLabel>
#include <QAction>
#include <QVBoxLayout>
#include <QMaemo5InformationBox>
#include <QMessageBox>
#include <QMenuBar>

DailymotionAccountsWindow::DailymotionAccountsWindow(StackedWindow *parent) :
    StackedWindow(parent),
    m_authRequest(0),
    m_userRequest(0),
    m_model(new DailymotionAccountModel(this)),
    m_view(new ListView(this)),
    m_addAction(new QAction(tr("Add account"), this)),
    m_removeAction(new QAction(tr("Remove"), this)),
    m_label(new QLabel(tr("Sign in to your Dailymotion account"), this)),
    m_revoke(false)
{
    setWindowTitle(tr("Accounts"));
    setCentralWidget(new QWidget);
    
    menuBar()->addAction(m_addAction);
        
    m_view->setModel(m_model);
    m_view->setModelColumn(1);
    m_view->setItemDelegate(new AccountDelegate(DailymotionAccountModel::ActiveRole, m_view));
    m_view->addAction(m_removeAction);
    
    m_label->setAlignment(Qt::AlignHCenter);
    
    m_layout = new QVBoxLayout(centralWidget());
    m_layout->addWidget(m_label);
    m_layout->addWidget(m_view);
    
    connect(m_model, SIGNAL(countChanged(int)), this, SLOT(onModelCountChanged(int)));
    connect(m_view, SIGNAL(activated(QModelIndex)), this, SLOT(selectAccount(QModelIndex)));
    connect(m_addAction, SIGNAL(triggered()), this, SLOT(showAuthDialog()));
    connect(m_removeAction, SIGNAL(triggered()), this, SLOT(revokeAccessToken()));
    
    onModelCountChanged(m_model->rowCount());
}

void DailymotionAccountsWindow::initAuthRequest() {
    if (!m_authRequest) {
        m_authRequest = new QDailymotion::AuthenticationRequest(this);
        m_authRequest->setClientId(Dailymotion::clientId());
        m_authRequest->setClientSecret(Dailymotion::clientSecret());
        m_authRequest->setRedirectUri(Dailymotion::redirectUri());
        m_authRequest->setScopes(Dailymotion::scopes());
        connect(m_authRequest, SIGNAL(finished()), this, SLOT(onAuthRequestFinished()));
    }
}

void DailymotionAccountsWindow::initUserRequest() {
    if (!m_userRequest) {
        m_userRequest = new QDailymotion::ResourcesRequest(this);
        m_userRequest->setClientId(Dailymotion::clientId());
        m_userRequest->setClientSecret(Dailymotion::clientSecret());
        
        if (m_authRequest) {
            const QVariantMap token = m_authRequest->result().toMap();
            m_userRequest->setAccessToken(token.value("access_token").toString());
            m_userRequest->setRefreshToken(token.value("refresh_token").toString());
        }
        
        connect(m_userRequest, SIGNAL(finished()), this, SLOT(onUserRequestFinished()));
    }
}

void DailymotionAccountsWindow::selectAccount(const QModelIndex &index) {
    if (m_model->selectAccount(index.row())) {
        QMaemo5InformationBox::information(this, tr("You have selected account '%1'")
                                                   .arg(index.data(DailymotionAccountModel::UsernameRole).toString()));
    }
    else {
        QMaemo5InformationBox::information(this, tr("Unable to select account '%1'")
                                                   .arg(index.data(DailymotionAccountModel::UsernameRole).toString()));
    }
}

void DailymotionAccountsWindow::showAuthDialog() {
    DailymotionAuthDialog dialog(this);
    dialog.login();

    if (dialog.exec() == QDialog::Accepted) {
        initAuthRequest();
        showProgressIndicator();
        m_revoke = false;
        m_authRequest->exchangeCodeForAccessToken(dialog.code());
    }
}

void DailymotionAccountsWindow::revokeAccessToken() {
    initAuthRequest();
    showProgressIndicator();
    m_revoke = true;
    m_authRequest->setAccessToken(m_view->currentIndex().data(DailymotionAccountModel::AccessTokenRole).toString());
    m_authRequest->setRefreshToken(m_view->currentIndex().data(DailymotionAccountModel::RefreshTokenRole).toString());
    m_authRequest->revokeAccessToken();
}

void DailymotionAccountsWindow::onAuthRequestFinished() {
    const QVariantMap result = m_authRequest->result().toMap();
    
    if (m_authRequest->status() == QDailymotion::AuthenticationRequest::Ready) {        
        if (m_revoke) {
            m_view->setEnabled(true);
            m_addAction->setEnabled(true);
            hideProgressIndicator();
            
            const QString username = m_view->currentIndex().data(DailymotionAccountModel::UsernameRole).toString();
            
            if (m_model->removeAccount(m_view->currentIndex().row())) {
                QMaemo5InformationBox::information(this, tr("Account '%1' removed").arg(username));
            }
            else {
                QMessageBox::critical(this, tr("Database error"), m_model->errorString());
            }
            
            return;
        }
        
        if (result.contains("access_token")) {
            initUserRequest();
            m_userRequest->get("/me", QVariantMap(), QStringList() << "id" << "screenname");
            return;
        }
    }
    
    if (m_revoke) {
        if (QMessageBox::question(this, tr("Error"),
            tr("Unable to revoke access token. Do you want to delete the account anyway?"),
            QMessageBox::Yes | QMessageBox::No) == QMessageBox::Yes) {
            const QString username = m_view->currentIndex().data(DailymotionAccountModel::UsernameRole).toString();
            
            if (m_model->removeAccount(m_view->currentIndex().row())) {
                QMaemo5InformationBox::information(this, tr("Account '%1' removed").arg(username));
            }
            else {
                QMessageBox::critical(this, tr("Database error"), m_model->errorString());
            }
        }
    }
    else {
        QMessageBox::critical(this, tr("Authentication failed"), Dailymotion::getErrorString(result));
    }
    
    m_view->setEnabled(true);
    m_addAction->setEnabled(true);
    hideProgressIndicator();
}

void DailymotionAccountsWindow::onUserRequestFinished() {
    if (m_userRequest->status() == QDailymotion::ResourcesRequest::Ready) {
        const QVariantMap user = m_userRequest->result().toMap();
        
        if (!user.isEmpty()) {
            m_view->setEnabled(true);
            m_addAction->setEnabled(true);
            hideProgressIndicator();
            
            if (m_model->addAccount(user.value("id").toString(), user.value("screenname").toString(),
                                    m_userRequest->accessToken(), m_userRequest->refreshToken(),
                                    Dailymotion::scopes().join(" "))) {
                QMaemo5InformationBox::information(this, tr("You are signed in to account '%1'")
                                                           .arg(user.value("screenname").toString()));
            }
            else {
                QMessageBox::critical(this, tr("Database error"), m_model->errorString());
            }
            
            return;
        }
    }
    
    QMessageBox::critical(this, tr("Authentication failed"),
                          Dailymotion::getErrorString(m_userRequest->result().toMap()));
    
    m_view->setEnabled(true);
    m_addAction->setEnabled(true);
    hideProgressIndicator();
}

void DailymotionAccountsWindow::onModelCountChanged(int count) {
    m_view->setContextMenuPolicy(count > 0 ? Qt::ActionsContextMenu : Qt::NoContextMenu);
}
