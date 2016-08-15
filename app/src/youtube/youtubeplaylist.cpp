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

#include "youtubeplaylist.h"
#include "logger.h"
#include "resources.h"
#include "youtube.h"
#include "youtubevideo.h"
#include <QDateTime>

YouTubePlaylist::YouTubePlaylist(QObject *parent) :
    CTPlaylist(parent),
    m_request(0),
    m_video(0)
{
    setService(Resources::YOUTUBE);
    connect(YouTube::instance(), SIGNAL(videoAddedToPlaylist(YouTubeVideo*, YouTubePlaylist*)),
            this, SLOT(onPlaylistUpdated(YouTubeVideo*, YouTubePlaylist*)));
    connect(YouTube::instance(), SIGNAL(videoRemovedFromPlaylist(YouTubeVideo*, YouTubePlaylist*)),
            this, SLOT(onPlaylistUpdated(YouTubeVideo*, YouTubePlaylist*)));
}

YouTubePlaylist::YouTubePlaylist(const QString &id, QObject *parent) :
    CTPlaylist(parent),
    m_request(0),
    m_video(0)
{
    setService(Resources::YOUTUBE);
    loadPlaylist(id);
    connect(YouTube::instance(), SIGNAL(videoAddedToPlaylist(YouTubeVideo*, YouTubePlaylist*)),
            this, SLOT(onPlaylistUpdated(YouTubeVideo*, YouTubePlaylist*)));
    connect(YouTube::instance(), SIGNAL(videoRemovedFromPlaylist(YouTubeVideo*, YouTubePlaylist*)),
            this, SLOT(onPlaylistUpdated(YouTubeVideo*, YouTubePlaylist*)));
}

YouTubePlaylist::YouTubePlaylist(const QVariantMap &playlist, QObject *parent) :
    CTPlaylist(parent),
    m_request(0),
    m_video(0)
{
    setService(Resources::YOUTUBE);
    loadPlaylist(playlist);
    connect(YouTube::instance(), SIGNAL(videoAddedToPlaylist(YouTubeVideo*, YouTubePlaylist*)),
            this, SLOT(onPlaylistUpdated(YouTubeVideo*, YouTubePlaylist*)));
    connect(YouTube::instance(), SIGNAL(videoRemovedFromPlaylist(YouTubeVideo*, YouTubePlaylist*)),
            this, SLOT(onPlaylistUpdated(YouTubeVideo*, YouTubePlaylist*)));
}

YouTubePlaylist::YouTubePlaylist(const YouTubePlaylist *playlist, QObject *parent) :
    CTPlaylist(playlist, parent),
    m_request(0),
    m_video(0),
    m_privacyStatus(playlist->privacyStatus())
{
    connect(YouTube::instance(), SIGNAL(videoAddedToPlaylist(YouTubeVideo*, YouTubePlaylist*)),
            this, SLOT(onPlaylistUpdated(YouTubeVideo*, YouTubePlaylist*)));
    connect(YouTube::instance(), SIGNAL(videoRemovedFromPlaylist(YouTubeVideo*, YouTubePlaylist*)),
            this, SLOT(onPlaylistUpdated(YouTubeVideo*, YouTubePlaylist*)));
}

QString YouTubePlaylist::errorString() const {
    return m_request ? YouTube::getErrorString(m_request->result().toMap()) : QString();
}

QString YouTubePlaylist::privacyStatus() const {
    return m_privacyStatus;
}

void YouTubePlaylist::setPrivacyStatus(const QString &s) {
    if (s != privacyStatus()) {
        m_privacyStatus = s;
        emit privacyStatusChanged();
    }
}

QYouTube::ResourcesRequest::Status YouTubePlaylist::status() const {
    return m_request ? m_request->status() : QYouTube::ResourcesRequest::Null;
}

void YouTubePlaylist::loadPlaylist(const QString &id) {
    if (status() == QYouTube::ResourcesRequest::Loading) {
        return;
    }
    
    initRequest();
    
    QVariantMap filters;
    filters["id"] = id;
    
    m_request->list("/playlists", QStringList() << "snippet" << "contentDetails", filters);
    connect(m_request, SIGNAL(finished()), this, SLOT(onPlaylistRequestFinished()));
    emit statusChanged(status());
}

void YouTubePlaylist::loadPlaylist(const QVariantMap &playlist) {
    const QVariantMap snippet = playlist.value("snippet").toMap();
    const QVariantMap status = playlist.value("status").toMap();
    const QVariantMap contentDetails = playlist.value("contentDetails").toMap();
    const QVariantMap thumbnails = snippet.value("thumbnails").toMap();
    
    setDate(QDateTime::fromString(snippet.value("publishedAt").toString(), Qt::ISODate).toString("dd MMM yyyy"));
    setDescription(snippet.value("description").toString());
    setId(YouTube::getPlaylistId(playlist));
    setLargeThumbnailUrl(thumbnails.value("high").toMap().value("url").toString());
    setThumbnailUrl(thumbnails.value("default").toMap().value("url").toString());
    setPrivacyStatus(status.value("privacyStatus").toString());
    setTitle(snippet.value("title").toString());
    setUserId(snippet.value("channelId").toString());
    setUsername(snippet.value("channelTitle").toString());
    setVideoCount(contentDetails.value("itemCount").toInt());    
}

void YouTubePlaylist::loadPlaylist(YouTubePlaylist *playlist) {
    CTPlaylist::loadPlaylist(playlist);
    setPrivacyStatus(playlist->privacyStatus());
}

void YouTubePlaylist::addVideo(YouTubeVideo *video) {
    if (status() == QYouTube::ResourcesRequest::Loading) {
        return;
    }
    
    Logger::log(QString("YouTubePlaylist::addVideo(). Video ID: %1, Playlist ID: %2").arg(video->id()).arg(id()),
                Logger::MediumVerbosity);
    initRequest();
    m_video = video;
    
    if (id().isEmpty()) {
        QVariantMap playlist;
        QVariantMap snippet;
        QVariantMap status;
        snippet["title"] = title();
        
        if (!description().isEmpty()) {
            snippet["description"] = description();
        }
        
        status["privacyStatus"] = privacyStatus();
        playlist["snippet"] = snippet;
        playlist["status"] = status;
        
        m_request->insert(playlist, "/playlists", QStringList() << "snippet" << "status");
        connect(m_request, SIGNAL(finished()), this, SLOT(onCreatePlaylistRequestFinished()));
    }
    else {
        QVariantMap resource;
        QVariantMap snippet;
        QVariantMap resourceId;
        resourceId["kind"] = "youtube#video";
        resourceId["videoId"] = m_video->id();
        snippet["playlistId"] = id();
        snippet["resourceId"] = resourceId;
        resource["snippet"] = snippet;
        
        m_request->insert(resource, "/playlistItems", QStringList() << "snippet");
        connect(m_request, SIGNAL(finished()), this, SLOT(onAddVideoRequestFinished()));
    }
    
    emit statusChanged(status());
}

void YouTubePlaylist::addVideo(const QVariantMap &playlist, YouTubeVideo *video) {
    loadPlaylist(playlist);
    addVideo(video);
}

void YouTubePlaylist::removeVideo(YouTubeVideo *video) {
    if (status() == QYouTube::ResourcesRequest::Loading) {
        return;
    }
    
    Logger::log(QString("YouTubePlaylist::removeVideo(). Video ID: %1, Playlist ID: %2").arg(video->id()).arg(id()),
                Logger::MediumVerbosity);
    initRequest();
    m_video = video;
    m_request->del(m_video->playlistItemId(), "/playlistItems");
    connect(m_request, SIGNAL(finished()), this, SLOT(onRemoveVideoRequestFinished()));
    emit statusChanged(status());
}

void YouTubePlaylist::initRequest() {
    if (!m_request) {
        m_request = new QYouTube::ResourcesRequest(this);
        m_request->setApiKey(YouTube::apiKey());
        m_request->setClientId(YouTube::clientId());
        m_request->setClientSecret(YouTube::clientSecret());
        m_request->setAccessToken(YouTube::accessToken());
        m_request->setRefreshToken(YouTube::refreshToken());
    
        connect(m_request, SIGNAL(accessTokenChanged(QString)), YouTube::instance(), SLOT(setAccessToken(QString)));
        connect(m_request, SIGNAL(refreshTokenChanged(QString)), YouTube::instance(), SLOT(setRefreshToken(QString)));
    }
}

void YouTubePlaylist::onPlaylistRequestFinished() {
    if (m_request->status() == QYouTube::ResourcesRequest::Ready) {
        const QVariantList list = m_request->result().toMap().value("items").toList();
        
        if (!list.isEmpty()) {
            loadPlaylist(list.first().toMap());
        }
    }
    
    disconnect(m_request, SIGNAL(finished()), this, SLOT(onPlaylistRequestFinished()));
    emit statusChanged(status());
}

void YouTubePlaylist::onCreatePlaylistRequestFinished() {
    disconnect(m_request, SIGNAL(finished()), this, SLOT(onCreatePlaylistRequestFinished()));
    
    if (m_request->status() == QYouTube::ResourcesRequest::Ready) {
        loadPlaylist(m_request->result().toMap());
        Logger::log("YouTubePlaylist::onCreatePlaylistRequestFinished(). Playlist created. ID: " + id(),
                    Logger::MediumVerbosity);
        emit YouTube::instance()->playlistCreated(this);
        
        if (m_video) {
            QVariantMap resource;
            QVariantMap snippet;
            QVariantMap resourceId;
            resourceId["kind"] = "youtube#video";
            resourceId["videoId"] = m_video->id();
            snippet["playlistId"] = id();
            snippet["resourceId"] = resourceId;
            resource["snippet"] = snippet;
        
            m_request->insert(resource, "/playlistItems", QStringList() << "snippet");
            connect(m_request, SIGNAL(finished()), this, SLOT(onAddVideoRequestFinished()));
            return;
        }
    }
    else {
        Logger::log("YouTubePlaylist::onCreatePlaylistRequestFinished(). Error: " + errorString());
    }
        
    emit statusChanged(status());
}

void YouTubePlaylist::onAddVideoRequestFinished() {
    if (m_request->status() == QYouTube::ResourcesRequest::Ready) {
        setVideoCount(videoCount() + 1);
        
        if (m_video) {
            if (videoCount() == 1) {
                setThumbnailUrl(m_video->thumbnailUrl());
                setLargeThumbnailUrl(m_video->largeThumbnailUrl());
            }
            
            Logger::log(QString("YouTubePlaylist::onAddVideoRequestFinished(). Video added. Video ID: %1, Playlist ID: %2").arg(m_video->id()).arg(id()), Logger::MediumVerbosity);
            emit YouTube::instance()->videoAddedToPlaylist(m_video, this);
        }
    }
    else {
        Logger::log("YouTubePlaylist::onAddVideoRequestFinished(). Error: " + errorString());
    }
    
    m_video = 0;
    disconnect(m_request, SIGNAL(finished()), this, SLOT(onAddVideoRequestFinished()));
    emit statusChanged(status());
}

void YouTubePlaylist::onRemoveVideoRequestFinished() {
    if (m_request->status() == QYouTube::ResourcesRequest::Ready) {
        setVideoCount(qMax(0, videoCount() - 1));
        
        if (m_video) {
            Logger::log(QString("YouTubePlaylist::onRemoveVideoRequestFinished(). Video removed. Video ID: %1, Playlist ID: %2").arg(m_video->id()).arg(id()), Logger::MediumVerbosity);
            emit YouTube::instance()->videoRemovedFromPlaylist(m_video, this);
        }
    }
    else {
        Logger::log("YouTubePlaylist::onRemoveVideoRequestFinished(). Error: " + errorString());
    }
    
    m_video = 0;
    disconnect(m_request, SIGNAL(finished()), this, SLOT(onRemoveVideoRequestFinished()));
    emit statusChanged(status());
}

void YouTubePlaylist::onPlaylistUpdated(YouTubeVideo*, YouTubePlaylist *playlist) {
    if ((playlist->id() == id()) && (playlist != this)) {
        loadPlaylist(playlist);
    }
}
