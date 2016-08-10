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

#include "pluginuser.h"
#include "pluginmanager.h"
#include "resources.h"

PluginUser::PluginUser(QObject *parent) :
    CTUser(parent),
    m_request(0)
{
}

PluginUser::PluginUser(const QString &service, const QString &id, QObject *parent) :
    CTUser(parent),
    m_request(0)
{
    loadUser(service, id);
}

PluginUser::PluginUser(const QString &service, const QVariantMap &user, QObject *parent) :
    CTUser(parent),
    m_request(0)
{
    loadUser(service, user);
}

PluginUser::PluginUser(const PluginUser *user, QObject *parent) :
    CTUser(user, parent),
    m_request(0),
    m_playlistsId(user->playlistsId()),
    m_videosId(user->videosId())
{
}

QString PluginUser::playlistsId() const {
    return m_playlistsId;
}

void PluginUser::setPlaylistsId(const QString &i) {
    if (i != playlistsId()) {
        m_playlistsId = i;
        emit playlistsIdChanged();
    }
}

QString PluginUser::videosId() const {
    return m_videosId;
}

void PluginUser::setVideosId(const QString &i) {
    if (i != videosId()) {
        m_videosId = i;
        emit videosIdChanged();
    }
}

QString PluginUser::errorString() const {
    return m_request ? m_request->errorString() : QString();
}

ResourcesRequest::Status PluginUser::status() const {
    return m_request ? m_request->status() : ResourcesRequest::Null;
}

void PluginUser::loadUser(const QString &service, const QString &id) {
    setService(service);
    
    if (status() == ResourcesRequest::Loading) {
        return;
    }
    
    if (ResourcesRequest *r = request()) {
        r->get(Resources::USER, id);
        emit statusChanged(status());
    }
}

void PluginUser::loadUser(const QString &service, const QVariantMap &user) {
    setService(service);
    setDescription(user.value("description").toString());
    setId(user.value("id").toString());
    setLargeThumbnailUrl(user.value("largeThumbnailUrl").toString());
    setPlaylistsId(user.value("playlistsId").toString());
    setThumbnailUrl(user.value("thumbnailUrl").toString());
    setUsername(user.value("username").toString());
    setVideosId(user.value("videosId").toString());
}

void PluginUser::loadUser(PluginUser *user) {
    CTUser::loadUser(user);
    setPlaylistsId(user->playlistsId());
    setVideosId(user->videosId());
}

ResourcesRequest* PluginUser::request() {
    if (!m_request) {
        m_request = PluginManager::instance()->createRequestForService(service(), this);

        if (m_request) {
            connect(m_request, SIGNAL(finished()), this, SLOT(onRequestFinished()));
        }
    }

    return m_request;
}  

void PluginUser::onRequestFinished() {
    if (m_request->status() == ResourcesRequest::Ready) {
        loadUser(service(), m_request->result().toMap());
    }
    
    emit statusChanged(status());
}
