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

#include "pluginvideo.h"
#include "pluginmanager.h"
#include "resources.h"
#include "utils.h"

PluginVideo::PluginVideo(QObject *parent) :
    CTVideo(parent),
    m_request(0)
{
}

PluginVideo::PluginVideo(const QString &service, const QString &id, QObject *parent) :
    CTVideo(parent),
    m_request(0)
{
    loadVideo(service, id);
}

PluginVideo::PluginVideo(const QString &service, const QVariantMap &video, QObject *parent) :
    CTVideo(parent),
    m_request(0)
{
    loadVideo(service, video);
}

PluginVideo::PluginVideo(const PluginVideo *video, QObject *parent) :
    CTVideo(video, parent),
    m_request(0),
    m_commentsId(video->commentsId()),
    m_relatedVideosId(video->relatedVideosId())
{
}

QString PluginVideo::commentsId() const {
    return m_commentsId;
}

void PluginVideo::setCommentsId(const QString &i) {
    if (i != commentsId()) {
        m_commentsId = i;
        emit commentsIdChanged();
    }
}

QString PluginVideo::relatedVideosId() const {
    return m_relatedVideosId;
}

void PluginVideo::setRelatedVideosId(const QString &i) {
    if (i != relatedVideosId()) {
        m_relatedVideosId = i;
        emit relatedVideosIdChanged();
    }
}

QString PluginVideo::errorString() const {
    return m_request ? m_request->errorString() : QString();
}

ResourcesRequest::Status PluginVideo::status() const {
    return m_request ? m_request->status() : ResourcesRequest::Null;
}

void PluginVideo::loadVideo(const QString &service, const QString &id) {
    setService(service);
    
    if (status() == ResourcesRequest::Loading) {
        return;
    }
    
    if (ResourcesRequest *r = request()) {
        r->get(Resources::VIDEO, id);
        emit statusChanged(status());
    }
}

void PluginVideo::loadVideo(const QString &service, const QVariantMap &video) {
    setService(service);
    setCommentsId(video.value("commentsId").toString());
    setDate(video.value("date").toString());
    setDescription(video.value("description").toString());
    setDownloadable(video.value("downloadable", true).toBool());
    setHasSubtitles(video.value("subtitles", false).toBool());
    setId(video.value("id").toString());
    setLargeThumbnailUrl(video.value("largeThumbnailUrl").toString());
    setRelatedVideosId(video.value("relatedVideosId").toString());
    setStreamUrl(video.value("streamUrl").toString());
    setThumbnailUrl(video.value("thumbnailUrl").toString());
    setTitle(video.value("title").toString());
    setUrl(video.value("url").toString());
    setUserId(video.value("userId").toString());
    setUsername(video.value("username").toString());
    setViewCount(video.value("viewCount").toLongLong());

    const QVariant &duration = video.value("duration");

    switch (duration.type()) {
    case QVariant::Int:
    case QVariant::LongLong:
    case QVariant::Double:
        setDuration(Utils::formatSecs(qMax(0, duration.toInt())));
        break;
    default:
        setDuration(duration.toString());
        break;
    }
}

void PluginVideo::loadVideo(PluginVideo *video) {
    CTVideo::loadVideo(video);
    setCommentsId(video->commentsId());
    setRelatedVideosId(video->relatedVideosId());
}

ResourcesRequest* PluginVideo::request() {
    if (!m_request) {
        m_request = PluginManager::instance()->createRequestForService(service(), this);

        if (m_request) {
            connect(m_request, SIGNAL(finished()), this, SLOT(onRequestFinished()));
        }
    }

    return m_request;
}

void PluginVideo::onRequestFinished() {
    if (m_request->status() == ResourcesRequest::Ready) {
        loadVideo(service(), m_request->result().toMap());
    }
    
    emit statusChanged(status());
}
