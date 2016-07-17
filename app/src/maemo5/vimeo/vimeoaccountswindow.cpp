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

#include "vimeoaccountswindow.h"
#include "accountdelegate.h"
#include "vimeo.h"
#include "vimeoaccountmodel.h"
#include "vimeoauthdialog.h"
#include "listview.h"
#include <qvimeo/authenticationrequest.h>
#include <QLabel>
#include <QAction>
#include <QVBoxLayout>
#include <QMaemo5InformationBox>
#include <QMessageBox>
#include <QMenuBar>

VimeoAccountsWindow::VimeoAccountsWindow(StackedWindow *parent) :
    StackedWindow(parent),
    m_authRequest(0),
    m_model(new VimeoAccountModel(this)),
    m_view(new ListView(this)),
    m_addAction(new QAction(tr("Add account"), this)),
    m_removeAction(new QAction(tr("Remove"), this)),
    m_label(new QLabel(tr("Sign in to your Vimeo account"), this))
{
    setWindowTitle(tr("Accounts"));
    setCentralWidget(new QWidget);
    
    menuBar()->addAction(m_addAction);
        
    m_view->setModel(m_model);
    m_view->setModelColumn(1);
    m_view->setItemDelegate(new AccountDelegate(VimeoAccountModel::ActiveRole, m_view));
    m_view->addAction(m_removeAction);
    
    m_label->setAlignment(Qt::AlignHCenter);
    
    m_layout = new QVBoxLayout(centralWidget());
    m_layout->addWidget(m_label);
    m_layout->addWidget(m_view);
    
    connect(m_model, SIGNAL(countChanged(int)), this, SLOT(onModelCountChanged(int)));
    connect(m_view, SIGNAL(activated(QModelIndex)), this, SLOT(selectAccount(QModelIndex)));
    connect(m_addAction, SIGNAL(triggered()), this, SLOT(showAuthDialog()));
    connect(m_removeAction, SIGNAL(triggered()), this, SLOT(removeAccount()));
    
    onModelCountChanged(m_model->rowCount());
}

void VimeoAccountsWindow::initAuthRequest() {
    if (!m_authRequest) {
        m_authRequest = new QVimeo::AuthenticationRequest(this);
        m_authRequest->setClientId(Vimeo::clientId());
        m_authRequest->setClientSecret(Vimeo::clientSecret());
        m_authRequest->setRedirectUri(Vimeo::redirectUri());
        m_authRequest->setScopes(Vimeo::scopes());
        connect(m_authRequest, SIGNAL(finished()), this, SLOT(onAuthRequestFinished()));
    }
}

void VimeoAccountsWindow::selectAccount(const QModelIndex &index) {
    if (m_model->selectAccount(index.row())) {
        QMaemo5InformationBox::information(this, tr("You have selected account '%1'")
                                                   .arg(index.data(VimeoAccountModel::UsernameRole).toString()));
    }
    else {
        QMaemo5InformationBox::information(this, tr("Unable to select account '%1'")
                                                   .arg(index.data(VimeoAccountModel::UsernameRole).toString()));
    }
}

void VimeoAccountsWindow::showAuthDialog() {
    VimeoAuthDialog dialog(this);
    dialog.login();

    if (dialog.exec() == QDialog::Accepted) {
        initAuthRequest();
        showProgressIndicator();
        m_authRequest->exchangeCodeForAccessToken(dialog.code());
    }
}

void VimeoAccountsWindow::removeAccount() {
    if (m_view->currentIndex().isValid()) {
        const QString username = m_view->currentIndex().data(VimeoAccountModel::UsernameRole).toString();
        
        if (m_model->removeAccount(m_view->currentIndex().row())) {
            QMaemo5InformationBox::information(this,
            tr("Account '%1' removed. Please visit the Vimeo website to revoke the access token").arg(username));
        }
        else {
            QMessageBox::critical(this, tr("Database error"), m_model->errorString());
        }
    }
}

void VimeoAccountsWindow::onAuthRequestFinished() {
    const QVariantMap result = m_authRequest->result().toMap();
    
    if (m_authRequest->status() == QVimeo::AuthenticationRequest::Ready) {
        const QVariantMap user = result.value("user").toMap();
        
        if (m_model->addAccount(user.value("uri").toString().section('/', -1), user.value("name").toString(),
                                result.value("access_token").toString(), result.value("scope").toString())) {
            QMaemo5InformationBox::information(this, tr("You are signed in to account '%1'")
                                                       .arg(user.value("name").toString()));
        }
        else {
            QMessageBox::critical(this, tr("Database error"), m_model->errorString());
        }        
    }
    else {
        QMessageBox::critical(this, tr("Authentication failed"), Vimeo::getErrorString(result));
    }
    
    m_view->setEnabled(true);
    m_addAction->setEnabled(true);
    hideProgressIndicator();
}

void VimeoAccountsWindow::onModelCountChanged(int count) {
    m_view->setContextMenuPolicy(count > 0 ? Qt::ActionsContextMenu : Qt::NoContextMenu);
}
