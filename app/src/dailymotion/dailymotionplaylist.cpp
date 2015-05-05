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

#include "dailymotionplaylist.h"
#include "dailymotion.h"
#include "dailymotionvideo.h"
#include "resources.h"
#include <QDateTime>
#include <QDebug>

DailymotionPlaylist::DailymotionPlaylist(QObject *parent) :
    Playlist(parent),
    m_request(0),
    m_video(0)
{
    setService(Resources::DAILYMOTION);
    connect(Dailymotion::instance(), SIGNAL(videoAddedToPlaylist(const DailymotionVideo*, const DailymotionPlaylist*)),
            this, SLOT(onPlaylistUpdated(const DailymotionVideo*, const DailymotionPlaylist*)));
    connect(Dailymotion::instance(), SIGNAL(videoRemovedFromPlaylist(const DailymotionVideo*, const DailymotionPlaylist*)),
            this, SLOT(onPlaylistUpdated(const DailymotionVideo*, const DailymotionPlaylist*)));
}

DailymotionPlaylist::DailymotionPlaylist(const QString &id, QObject *parent) :
    Playlist(parent),
    m_request(0),
    m_video(0)
{
    setService(Resources::DAILYMOTION);
    loadPlaylist(id);
    connect(Dailymotion::instance(), SIGNAL(videoAddedToPlaylist(const DailymotionVideo*, const DailymotionPlaylist*)),
            this, SLOT(onPlaylistUpdated(const DailymotionVideo*, const DailymotionPlaylist*)));
    connect(Dailymotion::instance(), SIGNAL(videoRemovedFromPlaylist(const DailymotionVideo*, const DailymotionPlaylist*)),
            this, SLOT(onPlaylistUpdated(const DailymotionVideo*, const DailymotionPlaylist*)));
}

DailymotionPlaylist::DailymotionPlaylist(const QVariantMap &playlist, QObject *parent) :
    Playlist(parent),
    m_request(0),
    m_video(0)
{
    setService(Resources::DAILYMOTION);
    loadPlaylist(playlist);
    connect(Dailymotion::instance(), SIGNAL(videoAddedToPlaylist(const DailymotionVideo*, const DailymotionPlaylist*)),
            this, SLOT(onPlaylistUpdated(const DailymotionVideo*, const DailymotionPlaylist*)));
    connect(Dailymotion::instance(), SIGNAL(videoRemovedFromPlaylist(const DailymotionVideo*, const DailymotionPlaylist*)),
            this, SLOT(onPlaylistUpdated(const DailymotionVideo*, const DailymotionPlaylist*)));
}

DailymotionPlaylist::DailymotionPlaylist(const DailymotionPlaylist *playlist, QObject *parent) :
    Playlist(playlist, parent),
    m_request(0),
    m_video(0)
{
    connect(Dailymotion::instance(), SIGNAL(videoAddedToPlaylist(const DailymotionVideo*, const DailymotionPlaylist*)),
            this, SLOT(onPlaylistUpdated(const DailymotionVideo*, const DailymotionPlaylist*)));
    connect(Dailymotion::instance(), SIGNAL(videoRemovedFromPlaylist(const DailymotionVideo*, const DailymotionPlaylist*)),
            this, SLOT(onPlaylistUpdated(const DailymotionVideo*, const DailymotionPlaylist*)));
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
    setLargeThumbnailUrl(playlist.value("thumbnail_url").toUrl());
    setThumbnailUrl(playlist.value("thumbnail_120_url").toUrl());
    setTitle(playlist.value("name").toString());
    setUserId(playlist.value("owner.id").toString());
    setUsername(playlist.value("owner.screenname").toString());
    setVideoCount(playlist.value("videos_total").toInt());    
}

void DailymotionPlaylist::addVideo(const DailymotionVideo *video) {
    if (status() == QDailymotion::ResourcesRequest::Loading) {
        return;
    }
    
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

void DailymotionPlaylist::removeVideo(const DailymotionVideo *video) {
    if (status() == QDailymotion::ResourcesRequest::Loading) {
        return;
    }
    
    initRequest();
    m_video = video;
    m_request->del(QString("/playlist/%1/videos/%2").arg(id()).arg(m_video->id()));
    connect(m_request, SIGNAL(finished()), this, SLOT(onRemoveVideoRequestFinished()));
    emit statusChanged(status());
}

void DailymotionPlaylist::initRequest() {
    if (!m_request) {
        m_request = new QDailymotion::ResourcesRequest(this);
        m_request->setClientId(Dailymotion::instance()->clientId());
        m_request->setClientSecret(Dailymotion::instance()->clientSecret());
        m_request->setAccessToken(Dailymotion::instance()->accessToken());
        m_request->setRefreshToken(Dailymotion::instance()->refreshToken());
    
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
        emit Dailymotion::instance()->playlistCreated(this);
        
        if (m_video) {
            m_request->insert(QString("/playlist/%1/videos/%2").arg(id()).arg(m_video->id()));
            connect(m_request, SIGNAL(finished()), this, SLOT(onAddVideoRequestFinished()));
            return;
        }
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
            
            emit Dailymotion::instance()->videoAddedToPlaylist(m_video, this);
        }
    }
    
    m_video = 0;
    
    disconnect(m_request, SIGNAL(finished()), this, SLOT(onAddVideoRequestFinished()));
    emit statusChanged(status());
}

void DailymotionPlaylist::onRemoveVideoRequestFinished() {
    if (m_request->status() == QDailymotion::ResourcesRequest::Ready) {
        setVideoCount(qMax(0, videoCount() - 1));
        
        if (m_video) {
            emit Dailymotion::instance()->videoRemovedFromPlaylist(m_video, this);
        }
    }
    
    m_video = 0;
    
    disconnect(m_request, SIGNAL(finished()), this, SLOT(onRemoveVideoRequestFinished()));
    emit statusChanged(status());
}

void DailymotionPlaylist::onPlaylistUpdated(const DailymotionVideo*, const DailymotionPlaylist *playlist) {
    if ((playlist->id() == id()) && (playlist != this)) {
        Playlist::loadPlaylist(playlist);
    }
}
