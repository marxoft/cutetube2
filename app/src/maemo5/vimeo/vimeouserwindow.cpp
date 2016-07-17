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

#include "vimeouserwindow.h"
#include "definitions.h"
#include "image.h"
#include "listview.h"
#include "navdelegate.h"
#include "resources.h"
#include "settings.h"
#include "textbrowser.h"
#include "utils.h"
#include "vimeo.h"
#include "vimeoplaylistswindow.h"
#include "vimeoplaylistwindow.h"
#include "vimeouserswindow.h"
#include "vimeovideoswindow.h"
#include "vimeovideowindow.h"
#include <qvimeo/urls.h>
#include <QScrollArea>
#include <QLabel>
#include <QPushButton>
#include <QGridLayout>
#include <QMessageBox>
#include <QStringListModel>
#include <QDesktopServices>

VimeoUserWindow::VimeoUserWindow(const QString &id, StackedWindow *parent) :
    StackedWindow(parent),
    m_user(new VimeoUser(this)),
    m_model(new QStringListModel(QStringList() << tr("Videos") << tr("Likes") << tr("Albums")
                                               << tr("Subscriptions"), this)),
    m_avatar(new Image(this)),
    m_view(new ListView(this)),
    m_scrollArea(new QScrollArea(this)),
    m_titleLabel(new QLabel(this)),
    m_statsLabel(new QLabel(this)),
    m_descriptionLabel(new TextBrowser(this)),
    m_subscribeButton(new QPushButton(tr("Subscribe"), this))
{
    loadBaseUi();
    connect(m_user, SIGNAL(statusChanged(QVimeo::ResourcesRequest::Status)),
            this, SLOT(onUserStatusChanged(QVimeo::ResourcesRequest::Status)));
            
    m_user->loadUser(id);
}

VimeoUserWindow::VimeoUserWindow(const VimeoUser *user, StackedWindow *parent) :
    StackedWindow(parent),
    m_user(new VimeoUser(user, this)),
    m_model(new QStringListModel(QStringList() << tr("Videos") << tr("Likes") << tr("Albums")
                                               << tr("Subscriptions"), this)),
    m_avatar(new Image(this)),
    m_view(new ListView(this)),
    m_scrollArea(new QScrollArea(this)),
    m_titleLabel(new QLabel(this)),
    m_statsLabel(new QLabel(this)),
    m_descriptionLabel(new TextBrowser(this)),
    m_subscribeButton(new QPushButton(this))
{
    loadBaseUi();
    loadUserUi();
    connect(m_user, SIGNAL(statusChanged(QVimeo::ResourcesRequest::Status)),
            this, SLOT(onUserUpdateStatusChanged(QVimeo::ResourcesRequest::Status)));
    connect(m_user, SIGNAL(subscribedChanged()), this, SLOT(onUserSubscribedChanged())); 
    
    if ((!m_user->isSubscribed()) && (!Vimeo::userId().isEmpty())) {
        m_user->checkIfSubscribed();
    }
}

void VimeoUserWindow::loadBaseUi() {
    setWindowTitle(tr("User"));
    setCentralWidget(new QWidget);
    
    m_view->setModel(m_model);
    m_view->setItemDelegate(new NavDelegate(m_view));
    
    m_avatar->setFixedSize(100, 100);
    
    m_statsLabel->setStyleSheet("font-size: 13pt");
    
    m_subscribeButton->setEnabled(false);
    
    QWidget *scrollWidget = new QWidget(m_scrollArea);
    QGridLayout *grid = new QGridLayout(scrollWidget);
    grid->addWidget(m_avatar, 0, 0, Qt::AlignLeft);
    grid->addWidget(m_titleLabel, 1, 0, Qt::AlignLeft | Qt::AlignTop);
    grid->addWidget(m_statsLabel, 1, 1, Qt::AlignRight | Qt::AlignTop);
    grid->addWidget(m_subscribeButton, 2, 0, Qt::AlignLeft);
    grid->addWidget(m_descriptionLabel, 3, 0, 1, 2, Qt::AlignTop);
    grid->setRowStretch(3, 1);
    grid->setColumnStretch(0, 1);
    grid->setContentsMargins(0, 0, 0, 0);
    m_scrollArea->setWidget(scrollWidget);
    m_scrollArea->setWidgetResizable(true);
    m_scrollArea->setFixedWidth(410);
    
    m_layout = new QHBoxLayout(centralWidget());
    m_layout->addWidget(m_scrollArea);
    m_layout->addWidget(m_view);
    m_layout->setStretch(1, 1);
    m_layout->setContentsMargins(0, 0, 0, 0);
    
    connect(m_view, SIGNAL(activated(QModelIndex)), this, SLOT(showWindow(QModelIndex)));
    connect(m_descriptionLabel, SIGNAL(anchorClicked(QUrl)), this, SLOT(showResource(QUrl)));
    connect(m_subscribeButton, SIGNAL(clicked()), this, SLOT(setUserSubscription()));
}

void VimeoUserWindow::loadUserUi() {
    m_subscribeButton->setEnabled((!Vimeo::userId().isEmpty())
                                  && (m_user->id() != Vimeo::userId())
                                  && (Vimeo::hasScope(QVimeo::INTERACT_SCOPE)));
    m_subscribeButton->setText(m_user->isSubscribed() ? tr("Unsubscribe") : tr("Subscribe"));
    m_titleLabel->setText(m_titleLabel->fontMetrics().elidedText(m_user->username(), Qt::ElideRight, 250));
    m_statsLabel->setText(tr("%1 subscribers").arg(Utils::formatLargeNumber(m_user->subscriberCount())));
    m_descriptionLabel->setHtml(Utils::toRichText(m_user->description()));
    m_avatar->setSource(m_user->largeThumbnailUrl());
}

void VimeoUserWindow::showFavourites() {
    QVariantMap filters;
    filters["per_page"] = MAX_RESULTS;
    
    VimeoVideosWindow *window = new VimeoVideosWindow(this);
    window->setWindowTitle(tr("%1's likes").arg(m_user->username()));
    window->list(QString("/users/%1/likes").arg(m_user->id()), filters);
    window->show();
}

void VimeoUserWindow::showPlaylists() {
    QVariantMap filters;
    filters["per_page"] = MAX_RESULTS;
    
    VimeoPlaylistsWindow *window = new VimeoPlaylistsWindow(this);
    window->setWindowTitle(tr("%1's albums").arg(m_user->username()));
    window->list(QString("/users/%1/albums").arg(m_user->id()), filters);
    window->show();
}

void VimeoUserWindow::showSubscriptions() {
    QVariantMap filters;
    filters["per_page"] = MAX_RESULTS;
    
    VimeoUsersWindow *window = new VimeoUsersWindow(this);
    window->setWindowTitle(tr("%1's subscriptions").arg(m_user->username()));
    window->list(QString("/users/%1/following").arg(m_user->id()), filters);
    window->show();
}

void VimeoUserWindow::showUploads() {
    QVariantMap filters;
    filters["per_page"] = MAX_RESULTS;
    
    VimeoVideosWindow *window = new VimeoVideosWindow(this);
    window->setWindowTitle(tr("%1's videos").arg(m_user->username()));
    window->list(QString("/users/%1/videos").arg(m_user->id()), filters);
    window->show();
}

void VimeoUserWindow::showResource(const QUrl &url) {
    const QVariantMap resource = Resources::getResourceFromUrl(url.toString());
    
    if (resource.value("service") != Resources::VIMEO) {
        QDesktopServices::openUrl(url);
        return;
    }
    
    if (resource.value("type") == Resources::PLAYLIST) {
        VimeoPlaylistWindow *window = new VimeoPlaylistWindow(resource.value("id").toString(), this);
        window->show();
    }
    else if (resource.value("type") == Resources::USER) {
        VimeoUserWindow *window = new VimeoUserWindow(resource.value("id").toString(), this);
        window->show();
    }
    else {
        VimeoVideoWindow *window = new VimeoVideoWindow(resource.value("id").toString(), this);
        window->show();
    }
}

void VimeoUserWindow::showWindow(const QModelIndex &index) {
    if (isBusy()) {
        return;
    }
    
    switch (index.row()) {
    case 0:
        showUploads();
        break;
    case 1:
        showFavourites();
        break;
    case 2:
        showPlaylists();
        break;
    case 3:
        showSubscriptions();
        break;
    default:
        break;
    }
}

void VimeoUserWindow::setUserSubscription() {
    if (isBusy()) {
        return;
    }
    
    if (m_user->isSubscribed()) {
        m_user->unsubscribe();
    }
    else {
        m_user->subscribe();
    }
}

void VimeoUserWindow::onUserStatusChanged(QVimeo::ResourcesRequest::Status status) {
    switch (status) {
    case QVimeo::ResourcesRequest::Loading:
        showProgressIndicator();
        return;
    case QVimeo::ResourcesRequest::Ready:
        loadUserUi();
        break;
    case QVimeo::ResourcesRequest::Failed:
        QMessageBox::critical(this, tr("Error"), m_user->errorString());
        break;
    default:
        break;
    }
    
    hideProgressIndicator();
    disconnect(m_user, SIGNAL(statusChanged(QVimeo::ResourcesRequest::Status)),
               this, SLOT(onUserStatusChanged(QVimeo::ResourcesRequest::Status)));
    connect(m_user, SIGNAL(statusChanged(QVimeo::ResourcesRequest::Status)),
            this, SLOT(onUserUpdateStatusChanged(QVimeo::ResourcesRequest::Status)));
    connect(m_user, SIGNAL(subscribedChanged()), this, SLOT(onUserSubscribedChanged())); 
    
    if ((!m_user->isSubscribed()) && (!Vimeo::userId().isEmpty())) {
        m_user->checkIfSubscribed();
    }
}

void VimeoUserWindow::onUserUpdateStatusChanged(QVimeo::ResourcesRequest::Status status) {
    switch (status) {
    case QVimeo::ResourcesRequest::Loading:
        showProgressIndicator();
        return;
    case QVimeo::ResourcesRequest::Failed:
        QMessageBox::critical(this, tr("Error"), m_user->errorString());
        break;
    default:
        break;
    }
    
    hideProgressIndicator();
}

void VimeoUserWindow::onUserSubscribedChanged() {
    m_subscribeButton->setText(m_user->isSubscribed() ? tr("Unsubscribe") : tr("Subscribe"));
}
