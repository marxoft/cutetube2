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

#include "pluginvideo.h"
#include "resources.h"

PluginVideo::PluginVideo(QObject *parent) :
    Video(parent),
    m_request(new ResourcesRequest(this))
{
    connect(m_request, SIGNAL(finished()), this, SLOT(onRequestFinished()));
}

PluginVideo::PluginVideo(const QString &service, const QString &id, QObject *parent) :
    Video(parent),
    m_request(new ResourcesRequest(this))
{
    loadVideo(service, id);
    connect(m_request, SIGNAL(finished()), this, SLOT(onRequestFinished()));
}

PluginVideo::PluginVideo(const QString &service, const QVariantMap &video, QObject *parent) :
    Video(parent),
    m_request(new ResourcesRequest(this))
{
    loadVideo(service, video);
    connect(m_request, SIGNAL(finished()), this, SLOT(onRequestFinished()));
}

PluginVideo::PluginVideo(const PluginVideo *video, QObject *parent) :
    Video(video, parent),
    m_request(new ResourcesRequest(this))
{
}

QString PluginVideo::errorString() const {
    return m_request->errorString();
}

ResourcesRequest::Status PluginVideo::status() const {
    return m_request->status();
}

void PluginVideo::loadVideo(const QString &service, const QString &id) {
    if (status() == ResourcesRequest::Loading) {
        return;
    }
    
    m_request->setService(service);
    m_request->get(Resources::VIDEO, id);

    emit statusChanged(status());
}

void PluginVideo::loadVideo(const QString &service, const QVariantMap &video) {
    setService(service);
    setDate(video.value("date").toString());
    setDescription(video.value("description").toString());
    setDuration(video.value("duration").toString());
    setId(video.value("id").toString());
    setLargeThumbnailUrl(video.value("largeThumbnailUrl").toString());
    setStreamUrl(video.value("streamUrl").toString());
    setThumbnailUrl(video.value("thumbnailUrl").toString());
    setTitle(video.value("title").toString());
    setUrl(video.value("url").toString());
    setUserId(video.value("userId").toString());
    setUsername(video.value("username").toString());
    setViewCount(video.value("viewCount").toInt());    
}

void PluginVideo::onRequestFinished() {
    if (m_request->status() == ResourcesRequest::Ready) {
        loadVideo(m_request->service(), m_request->result().toMap());
    }
    
    emit statusChanged(status());
}
