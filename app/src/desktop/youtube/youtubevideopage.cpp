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

#include "youtubevideopage.h"
#include "definitions.h"
#include "image.h"
#include "textbrowser.h"
#include "transfers.h"
#include "utils.h"
#include "videoplayerwindow.h"
#include "videothumbnail.h"
#include "youtube.h"
#include "youtubecommentspage.h"
#include "youtubedownloaddialog.h"
#include "youtubeuserpage.h"
#include "youtubevideospage.h"
#include <QGridLayout>
#include <QLabel>
#include <QMessageBox>
#include <QSplitter>
#include <QStackedWidget>
#include <QTabBar>
#include <QToolBar>

YouTubeVideoPage::YouTubeVideoPage(const QString &videoId, QWidget *parent) :
    Page(parent),
    m_video(new YouTubeVideo(this)),
    m_splitter(new QSplitter(Qt::Horizontal, this)),
    m_videoContainer(new QWidget(this)),
    m_thumbnail(new VideoThumbnail(m_videoContainer)),
    m_toolBar(new QToolBar(m_videoContainer)),
    m_likeAction(new QAction(QIcon::fromTheme(""), tr("&Like"), this)),
    m_dislikeAction(new QAction(QIcon::fromTheme(""), tr("&Dislike"), this)),
    m_favouriteAction(new QAction(QIcon::fromTheme(""), tr("&Favourite"), this)),
    m_playlistAction(new QAction(QIcon::fromTheme(""), tr("&Add to playlist"), this)),
    m_queueAction(new QAction(QIcon::fromTheme(""), tr("&Queue"), this)),
    m_downloadAction(new QAction(QIcon::fromTheme(""), tr("&Download"), this)),
    m_avatar(new Image(m_videoContainer)),
    m_usernameLabel(new QLabel(m_videoContainer)),
    m_dateLabel(new QLabel(m_videoContainer)),
    m_descriptionLabel(new TextBrowser(m_videoContainer)),
    m_videoLayout(new QGridLayout(m_videoContainer)),
    m_tabsContainer(new QWidget(this)),
    m_tabBar(new QTabBar(m_tabsContainer)),
    m_tabs(new QStackedWidget(m_tabsContainer)),
    m_relatedTab(0),
    m_commentsTab(0),
    m_tabsLayout(new QVBoxLayout(m_tabsContainer)),
    m_layout(new QVBoxLayout(this)),
    m_status(Null)
{
    setWindowTitle(tr("Video info"));
    
    m_toolBar->setMovable(false);
    m_toolBar->addAction(m_likeAction);
    m_toolBar->addAction(m_dislikeAction);
    m_toolBar->addSeparator();
    m_toolBar->addAction(m_favouriteAction);
    m_toolBar->addAction(m_playlistAction);
    m_toolBar->addSeparator();
    m_toolBar->addAction(m_queueAction);
    m_toolBar->addAction(m_downloadAction);
    
    m_avatar->setFixedSize(60, 60);
    
    m_videoLayout->addWidget(m_thumbnail, 0, 0, 1, 2);
    m_videoLayout->addWidget(m_toolBar, 1, 0, 1, 2);
    m_videoLayout->addWidget(m_avatar, 2, 0, 2, 1, Qt::AlignTop);
    m_videoLayout->addWidget(m_usernameLabel, 2, 1);
    m_videoLayout->addWidget(m_dateLabel, 3, 1);
    m_videoLayout->addWidget(m_descriptionLabel, 4, 0, 1, 2, Qt::AlignTop);
    m_videoLayout->setContentsMargins(0, 0, 0, 0);
        
    m_tabBar->addTab(tr("Related"));
    m_tabBar->addTab(tr("Comments"));
    
    m_tabsLayout->addWidget(m_tabBar);
    m_tabsLayout->addWidget(m_tabs);
    m_tabsLayout->setContentsMargins(0, 0, 0, 0);
    
    m_splitter->addWidget(m_videoContainer);
    m_splitter->addWidget(m_tabsContainer);
    m_splitter->setStretchFactor(1, 1);
    
    m_layout->addWidget(m_splitter);
    m_layout->setContentsMargins(0, 0, 0, 0);
    
    connect(m_video, SIGNAL(statusChanged(QYouTube::ResourcesRequest::Status)),
            this, SLOT(onVideoLoadingStatusChanged(QYouTube::ResourcesRequest::Status)));
    m_video->loadVideo(videoId);
}

YouTubeVideoPage::YouTubeVideoPage(const YouTubeVideo *video, QWidget *parent) :
    Page(parent),
    m_video(new YouTubeVideo(video, this)),
    m_splitter(new QSplitter(Qt::Horizontal, this)),
    m_videoContainer(new QWidget(this)),
    m_thumbnail(new VideoThumbnail(m_videoContainer)),
    m_toolBar(new QToolBar(m_videoContainer)),
    m_likeAction(new QAction(QIcon::fromTheme(""), tr("&Like"), this)),
    m_dislikeAction(new QAction(QIcon::fromTheme(""), tr("&Dislike"), this)),
    m_favouriteAction(new QAction(QIcon::fromTheme(""), tr("&Favourite"), this)),
    m_playlistAction(new QAction(QIcon::fromTheme(""), tr("&Add to playlist"), this)),
    m_queueAction(new QAction(QIcon::fromTheme(""), tr("&Queue"), this)),
    m_downloadAction(new QAction(QIcon::fromTheme(""), tr("&Download"), this)),
    m_avatar(new Image(m_videoContainer)),
    m_usernameLabel(new QLabel(m_videoContainer)),
    m_dateLabel(new QLabel(m_videoContainer)),
    m_descriptionLabel(new TextBrowser(m_videoContainer)),
    m_videoLayout(new QGridLayout(m_videoContainer)),
    m_tabsContainer(new QWidget(this)),
    m_tabBar(new QTabBar(m_tabsContainer)),
    m_tabs(new QStackedWidget(m_tabsContainer)),
    m_relatedTab(0),
    m_commentsTab(0),
    m_tabsLayout(new QVBoxLayout(m_tabsContainer)),
    m_layout(new QVBoxLayout(this)),
    m_status(Ready)
{
    setWindowTitle(tr("Video info"));
        
    m_toolBar->setMovable(false);
    m_toolBar->addAction(m_likeAction);
    m_toolBar->addAction(m_dislikeAction);
    m_toolBar->addSeparator();
    m_toolBar->addAction(m_favouriteAction);
    m_toolBar->addAction(m_playlistAction);
    m_toolBar->addSeparator();
    m_toolBar->addAction(m_queueAction);
    m_toolBar->addAction(m_downloadAction);
    
    m_avatar->setFixedSize(60, 60);
    
    m_videoLayout->addWidget(m_thumbnail, 0, 0, 1, 2);
    m_videoLayout->addWidget(m_toolBar, 1, 0, 1, 2);
    m_videoLayout->addWidget(m_avatar, 2, 0, 2, 1, Qt::AlignTop);
    m_videoLayout->addWidget(m_usernameLabel, 2, 1);
    m_videoLayout->addWidget(m_dateLabel, 3, 1);
    m_videoLayout->addWidget(m_descriptionLabel, 4, 0, 1, 2, Qt::AlignTop);
    m_videoLayout->setContentsMargins(0, 0, 0, 0);
        
    m_tabBar->addTab(tr("Related"));
    m_tabBar->addTab(tr("Comments"));
    
    m_tabsLayout->addWidget(m_tabBar);
    m_tabsLayout->addWidget(m_tabs);
    m_tabsLayout->setContentsMargins(0, 0, 0, 0);
    
    m_splitter->addWidget(m_videoContainer);
    m_splitter->addWidget(m_tabsContainer);
    m_splitter->setStretchFactor(1, 1);
    
    m_layout->addWidget(m_splitter);
    m_layout->setContentsMargins(0, 0, 0, 0);
    
    loadUi();
}

void YouTubeVideoPage::resizeEvent(QResizeEvent *event) {
    Page::resizeEvent(event);
    //const int height = qMin(event->size().height() / 2, 480);
    //const int width = height * 4 / 3;
    //m_thumbnail->resize(width, height);
}

YouTubeVideoPage::Status YouTubeVideoPage::status() const {
    return m_status;
}

void YouTubeVideoPage::setStatus(YouTubeVideoPage::Status s) {
    if (s != status()) {
        m_status = s;
        emit statusChanged(s);
    }
}

QString YouTubeVideoPage::statusText() const {
    return m_statusText;
}

void YouTubeVideoPage::setStatusText(const QString &text) {
    if (text != statusText()) {
        m_statusText = text;
        emit statusTextChanged(text);
    }
}

void YouTubeVideoPage::cancel() {}

void YouTubeVideoPage::reload() {
    m_video->loadVideo(m_video->id());
}

void YouTubeVideoPage::loadUi() {
    m_thumbnail->setSource(m_video->largeThumbnailUrl());
    m_thumbnail->setDuration(m_video->duration());
    
    m_descriptionLabel->setText(Utils::toRichText(m_video->description()));
    
    m_tabBar->setCurrentIndex(0);
    
    setWindowTitle(m_video->title());
    showRelatedTab();
}

void YouTubeVideoPage::setCurrentTab(int index) {
    switch (index) {
    case 0:
        showRelatedTab();
        break;
    case 1:
        showCommentsTab();
        break;
    default:
        break;
    }
}

void YouTubeVideoPage::showRelatedTab() {
    if (!m_relatedTab) {
        QVariantMap filters;
        filters["relatedToVideoId"] = m_video->id();
        QVariantMap params;
        params["type"] = "video";
        params["maxResults"] = MAX_RESULTS;
        m_relatedTab = new YouTubeVideosPage(m_tabs);
        //m_relatedTab->setStack(stack());
        m_relatedTab->list("/search", QStringList() << "snippet", filters, params);
        m_tabs->addWidget(m_relatedTab);
    }
    
    m_tabs->setCurrentWidget(m_relatedTab);
}

void YouTubeVideoPage::showCommentsTab() {
    if (!m_commentsTab) {
        QVariantMap filters;
        filters["videoId"] = m_video->id();
        QVariantMap params;
        params["maxResults"] = MAX_RESULTS;
        params["textFormat"] = "plainText";
        m_commentsTab = new YouTubeCommentsPage(m_tabs);
        //m_commentsTab->setStack(stack());
        m_commentsTab->list("/commentThreads", QStringList() << "snippet" << "replies", filters, params);
        m_tabs->addWidget(m_commentsTab);
    }
    
    m_tabs->setCurrentWidget(m_commentsTab);
}

void YouTubeVideoPage::onVideoLoadingStatusChanged(QYouTube::ResourcesRequest::Status status) {
    switch (status) {
    case QYouTube::ResourcesRequest::Loading:
        setStatus(Page::Loading);
        setStatusText(tr("Loading video"));
        return;
    case QYouTube::ResourcesRequest::Ready:
        setStatusText(tr("Ready"));
        loadUi();
        break;
    case QYouTube::ResourcesRequest::Canceled:
        setStatusText(tr("Canceled"));
        break;
    case QYouTube::ResourcesRequest::Failed:
        setStatusText(m_video->errorString());
        QMessageBox::critical(this, tr("Error"), m_video->errorString());
        break;
    default:
        break;
    }
    
    setStatus(Page::Ready);
}
