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

#include "vimeovideo.h"
#include "resources.h"
#include "utils.h"
#include "vimeo.h"
#include <QDateTime>

VimeoVideo::VimeoVideo(QObject *parent) :
    CTVideo(parent),
    m_request(0),
    m_favourite(false)
{
    setHasSubtitles(true);
    setService(Resources::VIMEO);
    connect(Vimeo::instance(), SIGNAL(videoFavourited(VimeoVideo*)),
            this, SLOT(onVideoUpdated(VimeoVideo*)));
    connect(Vimeo::instance(), SIGNAL(videoUnfavourited(VimeoVideo*)),
            this, SLOT(onVideoUpdated(VimeoVideo*)));
}

VimeoVideo::VimeoVideo(const QString &id, QObject *parent) :
    CTVideo(parent),
    m_request(0),
    m_favourite(false)
{
    setHasSubtitles(true);
    setService(Resources::VIMEO);
    loadVideo(id);
    connect(Vimeo::instance(), SIGNAL(videoFavourited(VimeoVideo*)),
            this, SLOT(onVideoUpdated(VimeoVideo*)));
    connect(Vimeo::instance(), SIGNAL(videoUnfavourited(VimeoVideo*)),
            this, SLOT(onVideoUpdated(VimeoVideo*)));
}

VimeoVideo::VimeoVideo(const QVariantMap &video, QObject *parent) :
    CTVideo(parent),
    m_request(0),
    m_favourite(false)
{
    setHasSubtitles(true);
    setService(Resources::VIMEO);
    loadVideo(video);
    connect(Vimeo::instance(), SIGNAL(videoFavourited(VimeoVideo*)),
            this, SLOT(onVideoUpdated(VimeoVideo*)));
    connect(Vimeo::instance(), SIGNAL(videoUnfavourited(VimeoVideo*)),
            this, SLOT(onVideoUpdated(VimeoVideo*)));
}

VimeoVideo::VimeoVideo(const VimeoVideo *video, QObject *parent) :
    CTVideo(video, parent),
    m_request(0),
    m_favourite(video->isFavourite())
{
    connect(Vimeo::instance(), SIGNAL(videoFavourited(VimeoVideo*)),
            this, SLOT(onVideoUpdated(VimeoVideo*)));
    connect(Vimeo::instance(), SIGNAL(videoUnfavourited(VimeoVideo*)),
            this, SLOT(onVideoUpdated(VimeoVideo*)));
}

QString VimeoVideo::errorString() const {
    return m_request ? Vimeo::getErrorString(m_request->result().toMap()) : QString();
}

bool VimeoVideo::isFavourite() const {
    return m_favourite;
}

void VimeoVideo::setFavourite(bool f) {
    if (f != isFavourite()) {
        m_favourite = f;
        emit favouriteChanged();
    }
}

qint64 VimeoVideo::favouriteCount() const {
    return m_favouriteCount;
}

void VimeoVideo::setFavouriteCount(qint64 c) {
    if (c != favouriteCount()) {
        m_favouriteCount = c;
        emit favouriteCountChanged();
    }
}

QVimeo::ResourcesRequest::Status VimeoVideo::status() const {
    return m_request ? m_request->status() : QVimeo::ResourcesRequest::Null;
}

void VimeoVideo::loadVideo(const QString &id) {
    if (status() == QVimeo::ResourcesRequest::Loading) {
        return;
    }
    
    initRequest();
    m_request->get("/videos/" + id);
    connect(m_request, SIGNAL(finished()), this, SLOT(onVideoRequestFinished()));
    emit statusChanged(status());
}

void VimeoVideo::loadVideo(const QVariantMap &video) {
    const QVariantMap user = video.value("user").toMap();
    const QString thumbnailId = video.value("pictures").toMap().value("uri").toString().section('/', -1);
    
    setDate(QDateTime::fromString(video.value("created_time").toString(), Qt::ISODate).toString("dd MMM yyyy"));
    setDescription(video.value("description").toString());
    setDuration(Utils::formatSecs(video.value("duration").toLongLong()));
    setFavouriteCount(video.value("metadata").toMap().value("connections").toMap().value("likes").toMap()
                      .value("count").toLongLong());
    setId(video.value("uri").toString().section('/', -1));
    setLargeThumbnailUrl(QString("https://i.vimeocdn.com/video/%1_640x360.jpg").arg(thumbnailId));
    setUrl("https://vimeo.com/" + id());
    setUserId(user.value("uri").toString().section('/', -1));
    setUsername(user.value("name").toString());
    setThumbnailUrl(QString("https://i.vimeocdn.com/video/%1_100x75.jpg").arg(thumbnailId));
    setTitle(video.value("name").toString());
    setViewCount(video.value("stats").toMap().value("plays").toLongLong());
}

void VimeoVideo::loadVideo(VimeoVideo *video) {
    CTVideo::loadVideo(video);
    setFavourite(video->isFavourite());
}
    
void VimeoVideo::favourite() {
    if (status() == QVimeo::ResourcesRequest::Loading) {
        return;
    }
    
    initRequest();
    m_request->insert("/me/likes/" + id());
    connect(m_request, SIGNAL(finished()), this, SLOT(onFavouriteRequestFinished()));
    emit statusChanged(status());
}

void VimeoVideo::unfavourite() {
    if (status() == QVimeo::ResourcesRequest::Loading) {
        return;
    }
    
    initRequest();
    m_request->del("/me/likes/" + id());
    connect(m_request, SIGNAL(finished()), this, SLOT(onUnfavouriteRequestFinished()));
    emit statusChanged(status());
}

void VimeoVideo::watchLater() {
    if (status() == QVimeo::ResourcesRequest::Loading) {
        return;
    }
    
    initRequest();
    m_request->insert("/me/watchlater/" + id());
    connect(m_request, SIGNAL(finished()), this, SLOT(onWatchLaterRequestFinished()));
    emit statusChanged(status());
}

void VimeoVideo::initRequest() {
    if (!m_request) {
        m_request = new QVimeo::ResourcesRequest(this);
        m_request->setClientId(Vimeo::clientId());
        m_request->setClientSecret(Vimeo::clientSecret());
        m_request->setAccessToken(Vimeo::accessToken());
    
        connect(m_request, SIGNAL(accessTokenChanged(QString)), Vimeo::instance(), SLOT(setAccessToken(QString)));
    }
}

void VimeoVideo::onVideoRequestFinished() {
    if (m_request->status() == QVimeo::ResourcesRequest::Ready) {
        loadVideo(m_request->result().toMap());
    }
    
    disconnect(m_request, SIGNAL(finished()), this, SLOT(onVideoRequestFinished()));
    emit statusChanged(status());
}

void VimeoVideo::onFavouriteRequestFinished() {
    if (m_request->status() == QVimeo::ResourcesRequest::Ready) {
        setFavourite(true);
        setFavouriteCount(favouriteCount() + 1);
        emit Vimeo::instance()->videoFavourited(this);
    }
    
    disconnect(m_request, SIGNAL(finished()), this, SLOT(onFavouriteRequestFinished()));
    emit statusChanged(status());
}

void VimeoVideo::onUnfavouriteRequestFinished() {
    if (m_request->status() == QVimeo::ResourcesRequest::Ready) {
        setFavourite(false);
        setFavouriteCount(favouriteCount() - 1);
        emit Vimeo::instance()->videoUnfavourited(this);
    }
    
    disconnect(m_request, SIGNAL(finished()), this, SLOT(onUnfavouriteRequestFinished()));
    emit statusChanged(status());
}

void VimeoVideo::onWatchLaterRequestFinished() {
    if (m_request->status() == QVimeo::ResourcesRequest::Ready) {
        emit Vimeo::instance()->videoWatchLater(this);
    }
    
    disconnect(m_request, SIGNAL(finished()), this, SLOT(onWatchLaterRequestFinished()));
    emit statusChanged(status());
}

void VimeoVideo::onVideoUpdated(VimeoVideo *video) {
    if ((video->id() == id()) && (video != this)) {
        loadVideo(video);
    }
}
