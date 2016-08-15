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

#include "vimeoplaylist.h"
#include "logger.h"
#include "resources.h"
#include "vimeo.h"
#include "vimeovideo.h"
#include <QDateTime>

VimeoPlaylist::VimeoPlaylist(QObject *parent) :
    CTPlaylist(parent),
    m_request(0),
    m_video(0)
{
    setService(Resources::VIMEO);
    connect(Vimeo::instance(), SIGNAL(videoAddedToPlaylist(VimeoVideo*, VimeoPlaylist*)),
            this, SLOT(onPlaylistUpdated(VimeoVideo*, VimeoPlaylist*)));
    connect(Vimeo::instance(), SIGNAL(videoRemovedFromPlaylist(VimeoVideo*, VimeoPlaylist*)),
            this, SLOT(onPlaylistUpdated(VimeoVideo*, VimeoPlaylist*)));
}

VimeoPlaylist::VimeoPlaylist(const QString &id, QObject *parent) :
    CTPlaylist(parent),
    m_request(0),
    m_video(0)
{
    setService(Resources::VIMEO);
    loadPlaylist(id);
    connect(Vimeo::instance(), SIGNAL(videoAddedToPlaylist(VimeoVideo*, VimeoPlaylist*)),
            this, SLOT(onPlaylistUpdated(VimeoVideo*, VimeoPlaylist*)));
    connect(Vimeo::instance(), SIGNAL(videoRemovedFromPlaylist(VimeoVideo*, VimeoPlaylist*)),
            this, SLOT(onPlaylistUpdated(VimeoVideo*, VimeoPlaylist*)));
}

VimeoPlaylist::VimeoPlaylist(const QVariantMap &playlist, QObject *parent) :
    CTPlaylist(parent),
    m_request(0),
    m_video(0)
{
    setService(Resources::VIMEO);
    loadPlaylist(playlist);
    connect(Vimeo::instance(), SIGNAL(videoAddedToPlaylist(VimeoVideo*, VimeoPlaylist*)),
            this, SLOT(onPlaylistUpdated(VimeoVideo*, VimeoPlaylist*)));
    connect(Vimeo::instance(), SIGNAL(videoRemovedFromPlaylist(VimeoVideo*, VimeoPlaylist*)),
            this, SLOT(onPlaylistUpdated(VimeoVideo*, VimeoPlaylist*)));
}

VimeoPlaylist::VimeoPlaylist(const VimeoPlaylist *playlist, QObject *parent) :
    CTPlaylist(playlist, parent),
    m_request(0),
    m_video(0),
    m_password(playlist->password()),
    m_privacy(playlist->privacy())
{
    connect(Vimeo::instance(), SIGNAL(videoAddedToPlaylist(VimeoVideo*, VimeoPlaylist*)),
            this, SLOT(onPlaylistUpdated(VimeoVideo*, VimeoPlaylist*)));
    connect(Vimeo::instance(), SIGNAL(videoRemovedFromPlaylist(VimeoVideo*, VimeoPlaylist*)),
            this, SLOT(onPlaylistUpdated(VimeoVideo*, VimeoPlaylist*)));
}

QString VimeoPlaylist::errorString() const {
    return m_request ? Vimeo::getErrorString(m_request->result().toMap()) : QString();
}

QString VimeoPlaylist::password() const {
    return m_password;
}

void VimeoPlaylist::setPassword(const QString &p) {
    if (p != password()) {
        m_password = p;
        emit passwordChanged();
    }
}

QString VimeoPlaylist::privacy() const {
    return m_privacy;
}

void VimeoPlaylist::setPrivacy(const QString &p) {
    if (p != privacy()) {
        m_privacy = p;
        emit privacyChanged();
    }
}

QVimeo::ResourcesRequest::Status VimeoPlaylist::status() const {
    return m_request ? m_request->status() : QVimeo::ResourcesRequest::Null;
}

void VimeoPlaylist::loadPlaylist(const QString &id) {
    if (status() == QVimeo::ResourcesRequest::Loading) {
        return;
    }
    
    initRequest();
    m_request->get("/albums/" + id);
    connect(m_request, SIGNAL(finished()), this, SLOT(onPlaylistRequestFinished()));
    emit statusChanged(status());
}

void VimeoPlaylist::loadPlaylist(const QVariantMap &playlist) {
    const QVariantMap user = playlist.value("user").toMap();
    const QString thumbnailId = playlist.value("pictures").toMap().value("uri").toString().section('/', -1);
    
    setDate(QDateTime::fromString(playlist.value("created_time").toString(), Qt::ISODate).toString("dd MMM yyyy"));
    setDescription(playlist.value("description").toString());
    setId(playlist.value("uri").toString().section('/', -1));
    setLargeThumbnailUrl(QString("https://i.vimeocdn.com/video/%1_640x360.jpg").arg(thumbnailId));
    setPrivacy(playlist.value("privacy").toMap().value("view").toString());
    setThumbnailUrl(QString("https://i.vimeocdn.com/video/%1_100x75.jpg").arg(thumbnailId));
    setTitle(playlist.value("name").toString());
    setUserId(user.value("uri").toString().section('/', -1));
    setUsername(user.value("name").toString());
    setVideoCount(playlist.value("metadata").toMap().value("connections").toMap().value("videos").toMap()
                  .value("total").toInt());    
}

void VimeoPlaylist::addVideo(VimeoVideo *video) {
    if (status() == QVimeo::ResourcesRequest::Loading) {
        return;
    }
    
    Logger::log(QString("VimeoPlaylist::addVideo(). Video ID: %1, Playlist ID: %2").arg(video->id()).arg(id()),
                Logger::MediumVerbosity);
    initRequest();
    m_video = video;
    
    if (id().isEmpty()) {
        QVariantMap playlist;
        playlist["name"] = title();
        
        if (!description().isEmpty()) {
            playlist["description"] = description();
        }
        
        if (privacy() == "password") {
            playlist["privacy"] = "password";
            playlist["password"] = password();
        }
        
        m_request->insert(playlist, "/me/albums");
        connect(m_request, SIGNAL(finished()), this, SLOT(onCreatePlaylistRequestFinished()));
    }
    else {
        m_request->insert(QString("/albums/%1/videos/%2").arg(id()).arg(m_video->id()));
        connect(m_request, SIGNAL(finished()), this, SLOT(onAddVideoRequestFinished()));
    }
    
    emit statusChanged(status());
}

void VimeoPlaylist::addVideo(const QVariantMap &playlist, VimeoVideo *video) {
    loadPlaylist(playlist);
    addVideo(video);
}

void VimeoPlaylist::removeVideo(VimeoVideo *video) {
    if (status() == QVimeo::ResourcesRequest::Loading) {
        return;
    }
    
    Logger::log(QString("VimeoPlaylist::removeVideo(). Video ID: %1, Playlist ID: %2").arg(video->id()).arg(id()),
                Logger::MediumVerbosity);
    initRequest();
    m_video = video;    
    m_request->del(QString("/albums/%1/videos/%2").arg(id()).arg(m_video->id()));
    connect(m_request, SIGNAL(finished()), this, SLOT(onRemoveVideoRequestFinished()));
    emit statusChanged(status());
}

void VimeoPlaylist::initRequest() {
    if (!m_request) {
        m_request = new QVimeo::ResourcesRequest(this);
        m_request->setClientId(Vimeo::clientId());
        m_request->setClientSecret(Vimeo::clientSecret());
        m_request->setAccessToken(Vimeo::accessToken());
    
        connect(m_request, SIGNAL(accessTokenChanged(QString)), Vimeo::instance(), SLOT(setAccessToken(QString)));
    }
}

void VimeoPlaylist::onPlaylistRequestFinished() {
    if (m_request->status() == QVimeo::ResourcesRequest::Ready) {
        loadPlaylist(m_request->result().toMap());
    }
    
    disconnect(m_request, SIGNAL(finished()), this, SLOT(onPlaylistRequestFinished()));
    emit statusChanged(status());
}

void VimeoPlaylist::onCreatePlaylistRequestFinished() {
    disconnect(m_request, SIGNAL(finished()), this, SLOT(onCreatePlaylistRequestFinished()));
    
    if (m_request->status() == QVimeo::ResourcesRequest::Ready) {
        loadPlaylist(m_request->result().toMap());
        Logger::log("VimeoPlaylist::onCreatePlaylistRequestFinished(). Playlist created. ID: " + id(),
                    Logger::MediumVerbosity);
        emit Vimeo::instance()->playlistCreated(this);
        
        if (m_video) {
            m_request->insert(QString("/albums/%1/videos/%2").arg(id()).arg(m_video->id()));
            connect(m_request, SIGNAL(finished()), this, SLOT(onAddVideoRequestFinished()));
            return;
        }
    }
    else {
        Logger::log("VimeoPlaylist::onCreatePlaylistRequestFinished(). Error: " + errorString());
    }
        
    emit statusChanged(status());
}

void VimeoPlaylist::onAddVideoRequestFinished() {
    if (m_request->status() == QVimeo::ResourcesRequest::Ready) {
        setVideoCount(videoCount() + 1);
        
        if (m_video) {
            if (videoCount() == 1) {
                setThumbnailUrl(m_video->thumbnailUrl());
                setLargeThumbnailUrl(m_video->largeThumbnailUrl());
            }
            
            Logger::log(QString("VimeoPlaylist::onAddVideoRequestFinished(). Video added. Video ID: %1, Playlist ID: %2").arg(m_video->id()).arg(id()), Logger::MediumVerbosity);
            emit Vimeo::instance()->videoAddedToPlaylist(m_video, this);
        }
    }
    else {
        Logger::log("VimeoPlaylist::onAddVideoRequestFinished(). Error: " + errorString());
    }
    
    m_video = 0;
    disconnect(m_request, SIGNAL(finished()), this, SLOT(onAddVideoRequestFinished()));
    emit statusChanged(status());
}

void VimeoPlaylist::onRemoveVideoRequestFinished() {
    if (m_request->status() == QVimeo::ResourcesRequest::Ready) {
        setVideoCount(qMax(0, videoCount() - 1));
        
        if (m_video) {
            Logger::log(QString("VimeoPlaylist::onRemoveVideoRequestFinished(). Video removed. Video ID: %1, Playlist ID: %2").arg(m_video->id()).arg(id()), Logger::MediumVerbosity);
            emit Vimeo::instance()->videoRemovedFromPlaylist(m_video, this);
        }
    }
    else {
        Logger::log("VimeoPlaylist::onRemoveVideoRequestFinished(). Error: " + errorString());
    }
    
    m_video = 0;
    disconnect(m_request, SIGNAL(finished()), this, SLOT(onRemoveVideoRequestFinished()));
    emit statusChanged(status());
}

void VimeoPlaylist::onPlaylistUpdated(VimeoVideo*, VimeoPlaylist *playlist) {
    if ((playlist->id() == id()) && (playlist != this)) {
        CTPlaylist::loadPlaylist(playlist);
    }
}
