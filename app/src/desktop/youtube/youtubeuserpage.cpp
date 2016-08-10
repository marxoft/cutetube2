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

#include "youtubeuserpage.h"
#include "banner.h"
#include "definitions.h"
#include "pagestack.h"
#include "textbrowser.h"
#include "utils.h"
#include "youtube.h"
#include "youtubeplaylistspage.h"
#include "youtubeuserspage.h"
#include "youtubevideospage.h"
#include <QMessageBox>
#include <QToolBar>
#include <QVBoxLayout>

YouTubeUserPage::YouTubeUserPage(const QString &userId, QWidget *parent) :
    Page(parent),
    m_user(new YouTubeUser(this)),
    m_banner(new Banner(this)),
    m_toolBar(new QToolBar(this)),
    m_uploadsAction(new QAction(QIcon::fromTheme(""), tr("Show &videos"), this)),
    m_favouritesAction(new QAction(QIcon::fromTheme(""), tr("Show &favourites"), this)),
    m_playlistsAction(new QAction(QIcon::fromTheme(""), tr("Show &playlists"), this)),
    m_subscriptionsAction(new QAction(QIcon::fromTheme(""), tr("Show &subscriptions"), this)),
    m_subscribeAction(new QAction(QIcon::fromTheme(""), tr("&Subscribe"), this)),
    m_descriptionLabel(new TextBrowser(this)),
    m_layout(new QVBoxLayout(this)),
    m_status(Null)
{
    m_toolBar->addAction(m_uploadsAction);
    m_toolBar->addAction(m_favouritesAction);
    m_toolBar->addAction(m_playlistsAction);
    m_toolBar->addAction(m_subscriptionsAction);
    m_toolBar->addSeparator();
    m_toolBar->addAction(m_subscribeAction);
        
    m_layout->addWidget(m_banner);
    m_layout->addWidget(m_toolBar);
    m_layout->addWidget(m_descriptionLabel);
    m_layout->setContentsMargins(0, 0, 0, 0);
    
    connect(m_user, SIGNAL(statusChanged(QYouTube::ResourcesRequest::Status)),
            this, SLOT(onUserLoadingStatusChanged(QYouTube::ResourcesRequest::Status)));
    connect(m_uploadsAction, SIGNAL(triggered()), this, SLOT(showUploads()));
    connect(m_favouritesAction, SIGNAL(triggered()), this, SLOT(showFavourites()));
    connect(m_playlistsAction, SIGNAL(triggered()), this, SLOT(showPlaylists()));
    connect(m_subscriptionsAction, SIGNAL(triggered()), this, SLOT(showSubscriptions()));
    
    m_user->loadUser(userId);
}

YouTubeUserPage::YouTubeUserPage(const YouTubeUser *user, QWidget *parent) :
    Page(parent),
    m_user(new YouTubeUser(user, this)),
    m_banner(new Banner(this)),
    m_toolBar(new QToolBar(this)),
    m_uploadsAction(new QAction(QIcon::fromTheme(""), tr("Show &videos"), this)),
    m_favouritesAction(new QAction(QIcon::fromTheme(""), tr("Show &favourites"), this)),
    m_playlistsAction(new QAction(QIcon::fromTheme(""), tr("Show &playlists"), this)),
    m_subscriptionsAction(new QAction(QIcon::fromTheme(""), tr("Show &subscriptions"), this)),
    m_subscribeAction(new QAction(QIcon::fromTheme(""), tr("&Subscribe"), this)),
    m_descriptionLabel(new TextBrowser(this)),
    m_layout(new QVBoxLayout(this)),
    m_status(Null)
{
    m_toolBar->addAction(m_uploadsAction);
    m_toolBar->addAction(m_favouritesAction);
    m_toolBar->addAction(m_playlistsAction);
    m_toolBar->addAction(m_subscriptionsAction);
    m_toolBar->addSeparator();
    m_toolBar->addAction(m_subscribeAction);
        
    m_layout->addWidget(m_banner);
    m_layout->addWidget(m_toolBar);
    m_layout->addWidget(m_descriptionLabel);
    m_layout->setContentsMargins(0, 0, 0, 0);
    
    loadUi();
    
    connect(m_uploadsAction, SIGNAL(triggered()), this, SLOT(showUploads()));
    connect(m_favouritesAction, SIGNAL(triggered()), this, SLOT(showFavourites()));
    connect(m_playlistsAction, SIGNAL(triggered()), this, SLOT(showPlaylists()));
    connect(m_subscriptionsAction, SIGNAL(triggered()), this, SLOT(showSubscriptions()));
}

YouTubeUserPage::Status YouTubeUserPage::status() const {
    return m_status;
}

void YouTubeUserPage::setStatus(YouTubeUserPage::Status s) {
    if (s != status()) {
        m_status = s;
        emit statusChanged(s);
    }
}

QString YouTubeUserPage::statusText() const {
    return m_statusText;
}

void YouTubeUserPage::setStatusText(const QString &text) {
    if (text != statusText()) {
        m_statusText = text;
        emit statusTextChanged(text);
    }
}

void YouTubeUserPage::reload() {
    m_user->loadUser(m_user->id());
}

void YouTubeUserPage::loadUi() {
    m_banner->setSource(m_user->bannerUrl());
    m_banner->setAvatarSource(m_user->thumbnailUrl());
        
    m_subscribeAction->setText(tr("%1 subscribers").arg(Utils::formatLargeNumber(m_user->subscriberCount())));
    
    m_descriptionLabel->setText(Utils::toRichText(m_user->description()));
    
    setWindowTitle(m_user->username());
    setStatus(Ready);
}

void YouTubeUserPage::showFavourites() {
    if (!m_user->relatedPlaylists().contains("favorites")) {
        QMessageBox::critical(this, tr("Not available"), tr("This channel does not have any favourites"));
        return;
    }
    
    if (PageStack *s = stack()) {
        QVariantMap filters;
        filters["playlistId"] = m_user->relatedPlaylists().value("favorites");
        QVariantMap params;
        params["maxResults"] = MAX_RESULTS;
        YouTubeVideosPage *page = new YouTubeVideosPage(this);
        page->setWindowTitle(tr("%1's favourites").arg(m_user->username()));
        page->list("/playlistItems", QStringList() << "snippet", filters, params);
        s->push(page);
    }
}

void YouTubeUserPage::showPlaylists() {
    if (PageStack *s = stack()) {
        QVariantMap filters;
        filters["channelId"] = m_user->id();
        QVariantMap params;
        params["maxResults"] = MAX_RESULTS;
        YouTubePlaylistsPage *page = new YouTubePlaylistsPage(this);
        page->setWindowTitle(tr("%1's playlists").arg(m_user->username()));
        page->list("/playlists", QStringList() << "snippet" << "contentDetails", filters, params);
        s->push(page);
    }
}

void YouTubeUserPage::showSubscriptions() {
    if (PageStack *s = stack()) {
        QVariantMap filters;
        filters["channelId"] = m_user->id();
        QVariantMap params;
        params["sort"] = "unread";
        params["maxResults"] = MAX_RESULTS;
        YouTubeUsersPage *page = new YouTubeUsersPage(this);
        page->setWindowTitle(tr("%1's subscriptions").arg(m_user->username()));
        page->list("/subscriptions", QStringList() << "snippet", filters, params);
        s->push(page);
    }
}

void YouTubeUserPage::showUploads() {
    if (!m_user->relatedPlaylists().contains("uploads")) {
        QMessageBox::critical(this, tr("Not available"), tr("This channel does not have any videos"));
        return;
    }
    
    if (PageStack *s = stack()) {
        QVariantMap filters;
        filters["playlistId"] = m_user->relatedPlaylists().value("uploads");
        QVariantMap params;
        params["maxResults"] = MAX_RESULTS;
        YouTubeVideosPage *page = new YouTubeVideosPage(this);
        page->setWindowTitle(tr("%1's videos").arg(m_user->username()));
        page->list("/playlistItems", QStringList() << "snippet", filters, params);
        s->push(page);
    }
}

void YouTubeUserPage::onUserLoadingStatusChanged(QYouTube::ResourcesRequest::Status status) {
    switch (status) {
    case QYouTube::ResourcesRequest::Loading:
        setStatus(Page::Loading);
        setStatusText(tr("Loading profile"));
        return;
    case QYouTube::ResourcesRequest::Ready:
        setStatusText(tr("Ready"));
        loadUi();
        break;
    case QYouTube::ResourcesRequest::Canceled:
        setStatusText(tr("Canceled"));
        break;
    case QYouTube::ResourcesRequest::Failed:
        setStatusText(m_user->errorString());
        QMessageBox::critical(this, tr("Error"), m_user->errorString());
        break;
    default:
        break;
    }
    
    setStatus(Page::Ready);
}
