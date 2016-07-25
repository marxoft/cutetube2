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

#include "youtubeaccountswindow.h"
#include "accountdelegate.h"
#include "listview.h"
#include "youtube.h"
#include "youtubeaccountmodel.h"
#include "youtubeauthdialog.h"
#include <qyoutube/authenticationrequest.h>
#include <qyoutube/resourcesrequest.h>
#include <QLabel>
#include <QAction>
#include <QVBoxLayout>
#include <QMaemo5InformationBox>
#include <QMessageBox>
#include <QMenuBar>

YouTubeAccountsWindow::YouTubeAccountsWindow(StackedWindow *parent) :
    StackedWindow(parent),
    m_authRequest(0),
    m_userRequest(0),
    m_model(new YouTubeAccountModel(this)),
    m_view(new ListView(this)),
    m_addAction(new QAction(tr("Add account"), this)),
    m_removeAction(new QAction(tr("Remove"), this)),
    m_label(new QLabel(tr("Sign in to your YouTube account"), this)),
    m_revoke(false)
{
    setWindowTitle(tr("Accounts"));
    setCentralWidget(new QWidget);
    
    menuBar()->addAction(m_addAction);
        
    m_view->setModel(m_model);
    m_view->setModelColumn(1);
    m_view->setItemDelegate(new AccountDelegate(YouTubeAccountModel::ActiveRole, m_view));
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

void YouTubeAccountsWindow::initAuthRequest() {
    if (!m_authRequest) {
        m_authRequest = new QYouTube::AuthenticationRequest(this);
        m_authRequest->setApiKey(YouTube::apiKey());
        m_authRequest->setClientId(YouTube::clientId());
        m_authRequest->setClientSecret(YouTube::clientSecret());
        m_authRequest->setScopes(YouTube::scopes());
        connect(m_authRequest, SIGNAL(finished()), this, SLOT(onAuthRequestFinished()));
    }
}

void YouTubeAccountsWindow::initUserRequest() {
    if (!m_userRequest) {
        m_userRequest = new QYouTube::ResourcesRequest(this);
        m_authRequest->setApiKey(YouTube::apiKey());
        m_userRequest->setClientId(YouTube::clientId());
        m_userRequest->setClientSecret(YouTube::clientSecret());
        
        if (m_authRequest) {
            const QVariantMap token = m_authRequest->result().toMap();
            m_userRequest->setAccessToken(token.value("access_token").toString());
            m_userRequest->setRefreshToken(token.value("refresh_token").toString());
        }
        
        connect(m_userRequest, SIGNAL(finished()), this, SLOT(onUserRequestFinished()));
    }
}

void YouTubeAccountsWindow::selectAccount(const QModelIndex &index) {
    if (m_model->selectAccount(index.row())) {
        QMaemo5InformationBox::information(this, tr("You have selected account '%1'")
                                                   .arg(index.data(YouTubeAccountModel::UsernameRole).toString()));
    }
    else {
        QMaemo5InformationBox::information(this, tr("Unable to select account '%1'")
                                                   .arg(index.data(YouTubeAccountModel::UsernameRole).toString()));
    }
}

void YouTubeAccountsWindow::showAuthDialog() {
    YouTubeAuthDialog dialog(this);
    dialog.login();

    if (dialog.exec() == QDialog::Accepted) {
        initAuthRequest();
        showProgressIndicator();
        m_revoke = false;
        m_authRequest->exchangeCodeForAccessToken(dialog.code());
    }
}

void YouTubeAccountsWindow::revokeAccessToken() {
    initAuthRequest();
    showProgressIndicator();
    m_revoke = true;
    m_authRequest->setAccessToken(m_view->currentIndex().data(YouTubeAccountModel::AccessTokenRole).toString());
    m_authRequest->setRefreshToken(m_view->currentIndex().data(YouTubeAccountModel::RefreshTokenRole).toString());
    m_authRequest->revokeAccessToken();
}

void YouTubeAccountsWindow::onAuthRequestFinished() {
    const QVariantMap result = m_authRequest->result().toMap();
    
    if (m_authRequest->status() == QYouTube::AuthenticationRequest::Ready) {        
        if (m_revoke) {
            m_view->setEnabled(true);
            m_addAction->setEnabled(true);
            hideProgressIndicator();
            
            const QString username = m_view->currentIndex().data(YouTubeAccountModel::UsernameRole).toString();
            
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
            QVariantMap filters;
            filters["mine"] = true;
            m_userRequest->list("/channels", QStringList() << "id" << "snippet" << "contentDetails", filters);
            return;
        }
    }
    
    if (m_revoke) {
        if (QMessageBox::question(this, tr("Error"),
            tr("Unable to revoke access token. Do you want to delete the account anyway?"),
            QMessageBox::Yes | QMessageBox::No) == QMessageBox::Yes) {
            const QString username = m_view->currentIndex().data(YouTubeAccountModel::UsernameRole).toString();
            
            if (m_model->removeAccount(m_view->currentIndex().row())) {
                QMaemo5InformationBox::information(this, tr("Account '%1' removed").arg(username));
            }
            else {
                QMessageBox::critical(this, tr("Database error"), m_model->errorString());
            }
        }
    }
    else {
        QMessageBox::critical(this, tr("Authentication failed"), YouTube::getErrorString(result));
    }
    
    m_view->setEnabled(true);
    m_addAction->setEnabled(true);
    hideProgressIndicator();
}

void YouTubeAccountsWindow::onUserRequestFinished() {
    if (m_userRequest->status() == QYouTube::ResourcesRequest::Ready) {
        const QVariantList list = m_userRequest->result().toMap().value("items").toList();
        
        if (!list.isEmpty()) {
            const QVariantMap user = list.first().toMap();
            m_view->setEnabled(true);
            m_addAction->setEnabled(true);
            hideProgressIndicator();
            
            const QString username = user.value("snippet").toMap().value("title").toString();
            
            if (m_model->addAccount(user.value("id").toString(), username,
                                    user.value("contentDetails").toMap().value("relatedPlaylists").toMap(),
                                    m_userRequest->accessToken(), m_userRequest->refreshToken(),
                                    YouTube::scopes().join(" "))) {
                QMaemo5InformationBox::information(this, tr("You are signed in to account '%1'")
                                                           .arg(username));
            }
            else {
                QMessageBox::critical(this, tr("Database error"), m_model->errorString());
            }
            
            return;
        }
    }
    
    QMessageBox::critical(this, tr("Authentication failed"),
                          YouTube::getErrorString(m_userRequest->result().toMap()));
    
    m_view->setEnabled(true);
    m_addAction->setEnabled(true);
    hideProgressIndicator();
}

void YouTubeAccountsWindow::onModelCountChanged(int count) {
    m_view->setContextMenuPolicy(count > 0 ? Qt::ActionsContextMenu : Qt::NoContextMenu);
}
