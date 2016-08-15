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

#include "dailymotionplaylist.h"
#include "dailymotion.h"
#include "logger.h"
#include "dailymotionvideo.h"
#include "resources.h"
#include <QDateTime>

DailymotionPlaylist::DailymotionPlaylist(QObject *parent) :
    CTPlaylist(parent),
    m_request(0),
    m_video(0)
{
    setService(Resources::DAILYMOTION);
    connect(Dailymotion::instance(), SIGNAL(videoAddedToPlaylist(DailymotionVideo*, DailymotionPlaylist*)),
            this, SLOT(onPlaylistUpdated(DailymotionVideo*, DailymotionPlaylist*)));
    connect(Dailymotion::instance(), SIGNAL(videoRemovedFromPlaylist(DailymotionVideo*, DailymotionPlaylist*)),
            this, SLOT(onPlaylistUpdated(DailymotionVideo*, DailymotionPlaylist*)));
}

DailymotionPlaylist::DailymotionPlaylist(const QString &id, QObject *parent) :
    CTPlaylist(parent),
    m_request(0),
    m_video(0)
{
    setService(Resources::DAILYMOTION);
    loadPlaylist(id);
    connect(Dailymotion::instance(), SIGNAL(videoAddedToPlaylist(DailymotionVideo*, DailymotionPlaylist*)),
            this, SLOT(onPlaylistUpdated(DailymotionVideo*, DailymotionPlaylist*)));
    connect(Dailymotion::instance(), SIGNAL(videoRemovedFromPlaylist(DailymotionVideo*, DailymotionPlaylist*)),
            this, SLOT(onPlaylistUpdated(DailymotionVideo*, DailymotionPlaylist*)));
}

DailymotionPlaylist::DailymotionPlaylist(const QVariantMap &playlist, QObject *parent) :
    CTPlaylist(parent),
    m_request(0),
    m_video(0)
{
    setService(Resources::DAILYMOTION);
    loadPlaylist(playlist);
    connect(Dailymotion::instance(), SIGNAL(videoAddedToPlaylist(DailymotionVideo*, DailymotionPlaylist*)),
            this, SLOT(onPlaylistUpdated(DailymotionVideo*, DailymotionPlaylist*)));
    connect(Dailymotion::instance(), SIGNAL(videoRemovedFromPlaylist(DailymotionVideo*, DailymotionPlaylist*)),
            this, SLOT(onPlaylistUpdated(DailymotionVideo*, DailymotionPlaylist*)));
}

DailymotionPlaylist::DailymotionPlaylist(const DailymotionPlaylist *playlist, QObject *parent) :
    CTPlaylist(playlist, parent),
    m_request(0),
    m_video(0)
{
    connect(Dailymotion::instance(), SIGNAL(videoAddedToPlaylist(DailymotionVideo*, DailymotionPlaylist*)),
            this, SLOT(onPlaylistUpdated(DailymotionVideo*, DailymotionPlaylist*)));
    connect(Dailymotion::instance(), SIGNAL(videoRemovedFromPlaylist(DailymotionVideo*, DailymotionPlaylist*)),
            this, SLOT(onPlaylistUpdated(DailymotionVideo*, DailymotionPlaylist*)));
}

QString DailymotionPlaylist::errorString() const {
    return m_request ? Dailymotion::getErrorString(m_request->result().toMap()) : QString();
}

QDailymotion::ResourcesRequest::Status DailymotionPlaylist::status() const {
    return m_request ? m_request->status() : QDailymotion::ResourcesRequest::Null;
}

void DailymotionPlaylist::loadPlaylist(const QString &id) {
    if (status() == QDailymotion::ResourcesRequest::Loading) {
        return;
    }
    
    initRequest();
    
    QVariantMap filters;
    filters["family_filter"] = false;
    m_request->get("/playlist/" + id, filters, Dailymotion::PLAYLIST_FIELDS);
    connect(m_request, SIGNAL(finished()), this, SLOT(onPlaylistRequestFinished()));
    emit statusChanged(status());
}

void DailymotionPlaylist::loadPlaylist(const QVariantMap &playlist) {
    setDate(QDateTime::fromTime_t(playlist.value("created_time").toLongLong()).toString("dd MMM yyyy"));
    setDescription(playlist.value("description").toString());
    setId(playlist.value("id").toString());
    setLargeThumbnailUrl(playlist.value("thumbnail_360_url").toString());
    setThumbnailUrl(playlist.value("thumbnail_120_url").toString());
    setTitle(playlist.value("name").toString());
    setUserId(playlist.value("owner.id").toString());
    setUsername(playlist.value("owner.screenname").toString());
    setVideoCount(playlist.value("videos_total").toInt());    
}

void DailymotionPlaylist::addVideo(DailymotionVideo *video) {
    if (status() == QDailymotion::ResourcesRequest::Loading) {
        return;
    }
    
    Logger::log(QString("DailymotionPlaylist::addVideo(). Video ID: %1, Playlist ID: %2").arg(video->id()).arg(id()),
                Logger::MediumVerbosity);
    initRequest();
    m_video = video;
    
    if (id().isEmpty()) {
        QVariantMap playlist;
        playlist["name"] = title();
        
        if (!description().isEmpty()) {
            playlist["description"] = description();
        }
        
        m_request->insert(playlist, "/me/playlists");
        connect(m_request, SIGNAL(finished()), this, SLOT(onCreatePlaylistRequestFinished()));
    }
    else {
        m_request->insert(QString("/playlist/%1/videos/%2").arg(id()).arg(m_video->id()));
        connect(m_request, SIGNAL(finished()), this, SLOT(onAddVideoRequestFinished()));
    }
    
    emit statusChanged(status());
}

void DailymotionPlaylist::addVideo(const QVariantMap &playlist, DailymotionVideo *video) {
    loadPlaylist(playlist);
    addVideo(video);
}

void DailymotionPlaylist::removeVideo(DailymotionVideo *video) {
    if (status() == QDailymotion::ResourcesRequest::Loading) {
        return;
    }
    
    Logger::log(QString("DailymotionPlaylist::removeVideo(). Video ID: %1, Playlist ID: %2").arg(video->id()).arg(id()),
                Logger::MediumVerbosity);
    initRequest();
    m_video = video;
    m_request->del(QString("/playlist/%1/videos/%2").arg(id()).arg(m_video->id()));
    connect(m_request, SIGNAL(finished()), this, SLOT(onRemoveVideoRequestFinished()));
    emit statusChanged(status());
}

void DailymotionPlaylist::initRequest() {
    if (!m_request) {
        m_request = new QDailymotion::ResourcesRequest(this);
        m_request->setClientId(Dailymotion::clientId());
        m_request->setClientSecret(Dailymotion::clientSecret());
        m_request->setAccessToken(Dailymotion::accessToken());
        m_request->setRefreshToken(Dailymotion::refreshToken());
    
        connect(m_request, SIGNAL(accessTokenChanged(QString)), Dailymotion::instance(), SLOT(setAccessToken(QString)));
        connect(m_request, SIGNAL(refreshTokenChanged(QString)), Dailymotion::instance(), SLOT(setRefreshToken(QString)));
    }
}

void DailymotionPlaylist::onPlaylistRequestFinished() {
    if (m_request->status() == QDailymotion::ResourcesRequest::Ready) {
        loadPlaylist(m_request->result().toMap());
    }
    
    disconnect(m_request, SIGNAL(finished()), this, SLOT(onPlaylistRequestFinished()));
    emit statusChanged(status());
}

void DailymotionPlaylist::onCreatePlaylistRequestFinished() {
    disconnect(m_request, SIGNAL(finished()), this, SLOT(onCreatePlaylistRequestFinished()));
    
    if (m_request->status() == QDailymotion::ResourcesRequest::Ready) {
        loadPlaylist(m_request->result().toMap());
        Logger::log("DailymotionPlaylist::onCreatePlaylistRequestFinished(). Playlist created. ID: " + id(),
                    Logger::MediumVerbosity);
        emit Dailymotion::instance()->playlistCreated(this);
        
        if (m_video) {
            m_request->insert(QString("/playlist/%1/videos/%2").arg(id()).arg(m_video->id()));
            connect(m_request, SIGNAL(finished()), this, SLOT(onAddVideoRequestFinished()));
            return;
        }
    }
    else {
        Logger::log("DailymotionPlaylist::onCreatePlaylistRequestFinished(). Error: " + errorString());
    }
    
    emit statusChanged(status());
}

void DailymotionPlaylist::onAddVideoRequestFinished() {
    if (m_request->status() == QDailymotion::ResourcesRequest::Ready) {
        setVideoCount(videoCount() + 1);
        
        if (m_video) {
            if (videoCount() == 1) {
                setThumbnailUrl(m_video->thumbnailUrl());
                setLargeThumbnailUrl(m_video->largeThumbnailUrl());
            }
            
            Logger::log(QString("DailymotionPlaylist::onAddVideoRequestFinished(). Video added. Video ID: %1, Playlist ID: %2").arg(m_video->id()).arg(id()), Logger::MediumVerbosity);
            emit Dailymotion::instance()->videoAddedToPlaylist(m_video, this);
        }
    }
    else {
        Logger::log("DailymotionPlaylist::onAddVideoRequestFinished(). Error: " + errorString());
    }
    
    m_video = 0;
    disconnect(m_request, SIGNAL(finished()), this, SLOT(onAddVideoRequestFinished()));
    emit statusChanged(status());
}

void DailymotionPlaylist::onRemoveVideoRequestFinished() {
    if (m_request->status() == QDailymotion::ResourcesRequest::Ready) {
        setVideoCount(qMax(0, videoCount() - 1));
        
        if (m_video) {
            Logger::log(QString("DailymotionPlaylist::onRemoveVideoRequestFinished(). Video removed. Video ID: %1, Playlist ID: %2").arg(m_video->id()).arg(id()), Logger::MediumVerbosity);
            emit Dailymotion::instance()->videoRemovedFromPlaylist(m_video, this);
        }
    }
    else {
        Logger::log("DailymotionPlaylist::onRemoveVideoRequestFinished(). Error: " + errorString());
    }
    
    m_video = 0;
    disconnect(m_request, SIGNAL(finished()), this, SLOT(onRemoveVideoRequestFinished()));
    emit statusChanged(status());
}

void DailymotionPlaylist::onPlaylistUpdated(DailymotionVideo*, DailymotionPlaylist *playlist) {
    if ((playlist->id() == id()) && (playlist != this)) {
        CTPlaylist::loadPlaylist(playlist);
    }
}
