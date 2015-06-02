/*
 * Copyright (C) 2015 Stuart Howarth <showarth@marxoft.co.uk>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License version 3 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "youtubeuserwindow.h"
#include "banner.h"
#include "definitions.h"
#include "listview.h"
#include "navdelegate.h"
#include "resources.h"
#include "settings.h"
#include "textbrowser.h"
#include "utils.h"
#include "youtube.h"
#include "youtubeplaylistswindow.h"
#include "youtubeplaylistwindow.h"
#include "youtubeuserswindow.h"
#include "youtubevideoswindow.h"
#include "youtubevideowindow.h"
#include <qyoutube/urls.h>
#include <QScrollArea>
#include <QLabel>
#include <QPushButton>
#include <QGridLayout>
#include <QMessageBox>
#include <QStringListModel>
#include <QDesktopServices>
#include <QMaemo5InformationBox>

YouTubeUserWindow::YouTubeUserWindow(const QString &id, StackedWindow *parent) :
    StackedWindow(parent),
    m_user(new YouTubeUser(this)),
    m_model(new QStringListModel(QStringList() << tr("Videos") << tr("Favourites") << tr("Playlists")
                                               << tr("Subscriptions"), this)),
    m_banner(new Banner(this)),
    m_view(new ListView(this)),
    m_scrollArea(new QScrollArea(this)),
    m_titleLabel(new QLabel(this)),
    m_statsLabel(new QLabel(this)),
    m_descriptionLabel(new TextBrowser(this)),
    m_subscribeButton(new QPushButton(tr("Subscribe"), this))
{
    loadBaseUi();
    connect(m_user, SIGNAL(statusChanged(QYouTube::ResourcesRequest::Status)),
            this, SLOT(onUserStatusChanged(QYouTube::ResourcesRequest::Status)));
            
    m_user->loadUser(id);
}

YouTubeUserWindow::YouTubeUserWindow(const YouTubeUser *user, StackedWindow *parent) :
    StackedWindow(parent),
    m_user(new YouTubeUser(user, this)),
    m_model(new QStringListModel(QStringList() << tr("Videos") << tr("Favourites") << tr("Playlists")
                                               << tr("Subscriptions"), this)),
    m_banner(new Banner(this)),
    m_view(new ListView(this)),
    m_scrollArea(new QScrollArea(this)),
    m_titleLabel(new QLabel(this)),
    m_statsLabel(new QLabel(this)),
    m_descriptionLabel(new TextBrowser(this)),
    m_subscribeButton(new QPushButton(this))
{
    loadBaseUi();
    loadUserUi();
    connect(m_user, SIGNAL(statusChanged(QYouTube::ResourcesRequest::Status)),
            this, SLOT(onUserUpdateStatusChanged(QYouTube::ResourcesRequest::Status)));
    connect(m_user, SIGNAL(subscribedChanged()), this, SLOT(onUserSubscribedChanged()));
    
    if ((!m_user->isSubscribed()) && (!YouTube::instance()->userId().isEmpty())) {
        m_user->checkIfSubscribed();
    }
}

void YouTubeUserWindow::loadBaseUi() {
    setWindowTitle(tr("Channel"));
    setCentralWidget(new QWidget);
    
    m_view->setModel(m_model);
    m_view->setItemDelegate(new NavDelegate(m_view));
    
    m_banner->setFixedSize(400, 100);
    
    m_statsLabel->setStyleSheet("font-size: 18px");
    
    m_subscribeButton->setEnabled(false);
    
    QWidget *scrollWidget = new QWidget(m_scrollArea);
    QGridLayout *grid = new QGridLayout(scrollWidget);
    grid->addWidget(m_banner, 0, 0, 1, 2, Qt::AlignLeft);
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

void YouTubeUserWindow::loadUserUi() {
    m_subscribeButton->setEnabled((!YouTube::instance()->userId().isEmpty())
                                  && (m_user->id() != YouTube::instance()->userId())
                                  && ((YouTube::instance()->hasScope(QYouTube::READ_WRITE_SCOPE))
                                  || (YouTube::instance()->hasScope(QYouTube::FORCE_SSL_SCOPE))));
    m_subscribeButton->setText(m_user->isSubscribed() ? tr("Unsubscribe") : tr("Subscribe"));
    m_titleLabel->setText(m_titleLabel->fontMetrics().elidedText(m_user->username(), Qt::ElideRight, 250));
    m_statsLabel->setText(tr("%1 %2\n%3 %4").arg(Utils::formatLargeNumber(m_user->subscriberCount()))
                                            .arg(tr("subscribers"))
                                            .arg(Utils::formatLargeNumber(m_user->viewCount()))
                                            .arg(tr("views")));
    m_descriptionLabel->setHtml(Utils::toRichText(m_user->description()));
    m_banner->setSource(m_user->bannerUrl());
    m_banner->setAvatarSource(m_user->thumbnailUrl());    
}

void YouTubeUserWindow::showFavourites() {
    if (!m_user->relatedPlaylists().contains("favorites")) {
        QMaemo5InformationBox::information(this, tr("This channel does not have any favourites"));
        return;
    }
    
    QVariantMap filters;
    filters["playlistId"] = m_user->relatedPlaylists().value("favorites");
    
    QVariantMap params;
    params["maxResults"] = MAX_RESULTS;
    
    YouTubeVideosWindow *window = new YouTubeVideosWindow(this);
    window->setWindowTitle(tr("%1's favourites").arg(m_user->username()));
    window->list("/playlistItems", QStringList() << "snippet", filters, params);
    window->show();
}

void YouTubeUserWindow::showPlaylists() {
    QVariantMap filters;
    filters["channelId"] = m_user->id();
    
    QVariantMap params;
    params["maxResults"] = MAX_RESULTS;
    
    YouTubePlaylistsWindow *window = new YouTubePlaylistsWindow(this);
    window->setWindowTitle(tr("%1's playlists").arg(m_user->username()));
    window->list("/playlists", QStringList() << "snippet" << "contentDetails", filters, params);
    window->show();
}

void YouTubeUserWindow::showSubscriptions() {
    QVariantMap filters;
    filters["channelId"] = m_user->id();
    
    QVariantMap params;
    params["sort"] = "unread";
    params["maxResults"] = MAX_RESULTS;
    
    YouTubeUsersWindow *window = new YouTubeUsersWindow(this);
    window->setWindowTitle(tr("%1's subscriptions").arg(m_user->username()));
    window->list("/subscriptions", QStringList() << "snippet", filters, params);
    window->show();
}

void YouTubeUserWindow::showUploads() {
    if (!m_user->relatedPlaylists().contains("uploads")) {
        QMaemo5InformationBox::information(this, tr("This channel does not have any videos"));
        return;
    }
    
    QVariantMap filters;
    filters["playlistId"] = m_user->relatedPlaylists().value("uploads");
    
    QVariantMap params;
    params["maxResults"] = MAX_RESULTS;
    
    YouTubeVideosWindow *window = new YouTubeVideosWindow(this);
    window->setWindowTitle(tr("%1's videos").arg(m_user->username()));
    window->list("/playlistItems", QStringList() << "snippet", filters, params);
    window->show();
}

void YouTubeUserWindow::showResource(const QUrl &url) {
    QVariantMap resource = Resources::getResourceFromUrl(url.toString());
    
    if (resource.value("service") != Resources::YOUTUBE) {
        QDesktopServices::openUrl(url);
        return;
    }
    
    if (resource.value("type") == Resources::PLAYLIST) {
        YouTubePlaylistWindow *window = new YouTubePlaylistWindow(resource.value("id").toString(), this);
        window->show();
    }
    else if (resource.value("type") == Resources::USER) {
        YouTubeUserWindow *window = new YouTubeUserWindow(resource.value("id").toString(), this);
        window->show();
    }
    else {
        YouTubeVideoWindow *window = new YouTubeVideoWindow(resource.value("id").toString(), this);
        window->show();
    }
}

void YouTubeUserWindow::showWindow(const QModelIndex &index) {
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

void YouTubeUserWindow::setUserSubscription() {
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

void YouTubeUserWindow::onUserStatusChanged(QYouTube::ResourcesRequest::Status status) {
    switch (status) {
    case QYouTube::ResourcesRequest::Loading:
        showProgressIndicator();
        return;
    case QYouTube::ResourcesRequest::Ready:
        loadUserUi();
        break;
    case QYouTube::ResourcesRequest::Failed:
        QMessageBox::critical(this, tr("Error"), m_user->errorString());
        break;
    default:
        break;
    }
    
    hideProgressIndicator();
    disconnect(m_user, SIGNAL(statusChanged(QYouTube::ResourcesRequest::Status)),
               this, SLOT(onUserStatusChanged(QYouTube::ResourcesRequest::Status)));
    connect(m_user, SIGNAL(statusChanged(QYouTube::ResourcesRequest::Status)),
            this, SLOT(onUserUpdateStatusChanged(QYouTube::ResourcesRequest::Status)));
    connect(m_user, SIGNAL(subscribedChanged()), this, SLOT(onUserSubscribedChanged())); 
    
    if ((!m_user->isSubscribed()) && (!YouTube::instance()->userId().isEmpty())) {
        m_user->checkIfSubscribed();
    }
}

void YouTubeUserWindow::onUserUpdateStatusChanged(QYouTube::ResourcesRequest::Status status) {
    switch (status) {
    case QYouTube::ResourcesRequest::Loading:
        showProgressIndicator();
        return;
    case QYouTube::ResourcesRequest::Failed:
        QMessageBox::critical(this, tr("Error"), m_user->errorString());
        break;
    default:
        break;
    }
    
    hideProgressIndicator();
}

void YouTubeUserWindow::onUserSubscribedChanged() {
    m_subscribeButton->setText(m_user->isSubscribed() ? tr("Unsubscribe") : tr("Subscribe"));
}
