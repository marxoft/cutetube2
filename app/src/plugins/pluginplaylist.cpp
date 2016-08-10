/*
 * Copyright (C) 2016 Stuart Howarth <showarth@marxoft.co.uk>
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms and conditions of the GNU General Public License,
 * version 3, as published by the Free Software Foundation.
 *
 * This program is distributed in the hope it will be useful, but WITHOUT ANY
 * WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software Foundation,
 * Inc., 51 Franklin St - Fifth Floor, Boston, MA 02110-1301 USA.
 */

#include "pluginplaylist.h"
#include "pluginmanager.h"
#include "resources.h"

PluginPlaylist::PluginPlaylist(QObject *parent) :
    CTPlaylist(parent),
    m_request(0)
{
}

PluginPlaylist::PluginPlaylist(const QString &service, const QString &id, QObject *parent) :
    CTPlaylist(parent),
    m_request(0)
{
    loadPlaylist(service, id);
}

PluginPlaylist::PluginPlaylist(const QString &service, const QVariantMap &playlist, QObject *parent) :
    CTPlaylist(parent),
    m_request(0)
{
    loadPlaylist(service, playlist);
}

PluginPlaylist::PluginPlaylist(const PluginPlaylist *playlist, QObject *parent) :
    CTPlaylist(playlist, parent),
    m_request(0),
    m_videosId(playlist->videosId())
{
}

QString PluginPlaylist::videosId() const {
    return m_videosId;
}

void PluginPlaylist::setVideosId(const QString &i) {
    if (i != videosId()) {
        m_videosId = i;
        emit videosIdChanged();
    }
}

QString PluginPlaylist::errorString() const {
    return m_request ? m_request->errorString() : QString();
}

ResourcesRequest::Status PluginPlaylist::status() const {
    return m_request ? m_request->status() : ResourcesRequest::Null;
}

void PluginPlaylist::loadPlaylist(const QString &service, const QString &id) {
    setService(service);
    
    if (status() == ResourcesRequest::Loading) {
        return;
    }
    
    if (ResourcesRequest *r = request()) {
        r->get(Resources::PLAYLIST, id);
        emit statusChanged(status());
    }
}

void PluginPlaylist::loadPlaylist(const QString &service, const QVariantMap &playlist) {
    setService(service);
    setDate(playlist.value("date").toString());
    setDescription(playlist.value("description").toString());
    setId(playlist.value("id").toString());
    setLargeThumbnailUrl(playlist.value("largeThumbnailUrl").toString());
    setThumbnailUrl(playlist.value("thumbnailUrl").toString());
    setTitle(playlist.value("title").toString());
    setUserId(playlist.value("userId").toString());
    setUsername(playlist.value("username").toString());
    setVideoCount(playlist.value("videoCount", -1).toInt());
    setVideosId(playlist.value("videosId").toString());
}

void PluginPlaylist::loadPlaylist(PluginPlaylist *playlist) {
    CTPlaylist::loadPlaylist(playlist);
    setVideosId(playlist->videosId());
}

ResourcesRequest* PluginPlaylist::request() {
    if (!m_request) {
        m_request = PluginManager::instance()->createRequestForService(service(), this);

        if (m_request) {
            connect(m_request, SIGNAL(finished()), this, SLOT(onRequestFinished()));
        }
    }

    return m_request;
}

void PluginPlaylist::onRequestFinished() {
    if (m_request->status() == ResourcesRequest::Ready) {
        loadPlaylist(service(), m_request->result().toMap());
    }
    
    emit statusChanged(status());
}
