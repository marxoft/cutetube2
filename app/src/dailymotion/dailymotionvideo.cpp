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

#include "dailymotionvideo.h"
#include "dailymotion.h"
#include "resources.h"
#include "utils.h"
#include <QDateTime>

DailymotionVideo::DailymotionVideo(QObject *parent) :
    CTVideo(parent),
    m_request(0),
    m_favourite(false)
{
    setService(Resources::DAILYMOTION);
    connect(Dailymotion::instance(), SIGNAL(videoFavourited(DailymotionVideo*)),
            this, SLOT(onVideoUpdated(DailymotionVideo*)));
    connect(Dailymotion::instance(), SIGNAL(videoUnfavourited(DailymotionVideo*)),
            this, SLOT(onVideoUpdated(DailymotionVideo*)));
}

DailymotionVideo::DailymotionVideo(const QString &id, QObject *parent) :
    CTVideo(parent),
    m_request(0),
    m_favourite(false)
{
    setService(Resources::DAILYMOTION);
    loadVideo(id);
    connect(Dailymotion::instance(), SIGNAL(videoFavourited(DailymotionVideo*)),
            this, SLOT(onVideoUpdated(DailymotionVideo*)));
    connect(Dailymotion::instance(), SIGNAL(videoUnfavourited(DailymotionVideo*)),
            this, SLOT(onVideoUpdated(DailymotionVideo*)));
}

DailymotionVideo::DailymotionVideo(const QVariantMap &video, QObject *parent) :
    CTVideo(parent),
    m_request(0),
    m_favourite(false)
{
    setService(Resources::DAILYMOTION);
    loadVideo(video);
    connect(Dailymotion::instance(), SIGNAL(videoFavourited(DailymotionVideo*)),
            this, SLOT(onVideoUpdated(DailymotionVideo*)));
    connect(Dailymotion::instance(), SIGNAL(videoUnfavourited(DailymotionVideo*)),
            this, SLOT(onVideoUpdated(DailymotionVideo*)));
}

DailymotionVideo::DailymotionVideo(const DailymotionVideo *video, QObject *parent) :
    CTVideo(video, parent),
    m_request(0),
    m_favourite(video->isFavourite())
{
    connect(Dailymotion::instance(), SIGNAL(videoFavourited(DailymotionVideo*)),
            this, SLOT(onVideoUpdated(DailymotionVideo*)));
    connect(Dailymotion::instance(), SIGNAL(videoUnfavourited(DailymotionVideo*)),
            this, SLOT(onVideoUpdated(DailymotionVideo*)));
}

QString DailymotionVideo::errorString() const {
    return m_request ? Dailymotion::getErrorString(m_request->result().toMap()) : QString();
}

bool DailymotionVideo::isFavourite() const {
    return m_favourite;
}

void DailymotionVideo::setFavourite(bool f) {
    if (f != isFavourite()) {
        m_favourite = f;
        emit favouriteChanged();
    }
}

QDailymotion::ResourcesRequest::Status DailymotionVideo::status() const {
    return m_request ? m_request->status() : QDailymotion::ResourcesRequest::Null;
}

void DailymotionVideo::loadVideo(const QString &id) {
    if (status() == QDailymotion::ResourcesRequest::Loading) {
        return;
    }
    
    initRequest();
    
    QVariantMap filters;
    filters["family_filter"] = false;
    
    m_request->get("/video/" + id, filters, Dailymotion::VIDEO_FIELDS);
    connect(m_request, SIGNAL(finished()), this, SLOT(onVideoRequestFinished()));
    emit statusChanged(status());
}

void DailymotionVideo::loadVideo(const QVariantMap &video) {
    setDate(QDateTime::fromTime_t(video.value("created_time").toLongLong()).toString("dd MMM yyyy"));
    setDescription(video.value("description").toString());
    setDuration(Utils::formatSecs(video.value("duration").toLongLong()));
    setFavourite(video.value("favorited_at").toLongLong() > 0);
    setId(video.value("id").toString());
    setLargeThumbnailUrl(video.value("thumbnail_url").toString());
    setUrl(video.value("url").toString());
    setUserId(video.value("owner.id").toString());
    setUsername(video.value("owner.screenname").toString());
    setThumbnailUrl(video.value("thumbnail_120_url").toString());
    setTitle(video.value("title").toString());
    setViewCount(video.value("views_total").toLongLong());
}

void DailymotionVideo::loadVideo(DailymotionVideo *video) {
    CTVideo::loadVideo(video);
    setFavourite(video->isFavourite());
}
    
void DailymotionVideo::favourite() {
    if (status() == QDailymotion::ResourcesRequest::Loading) {
        return;
    }
    
    initRequest();
    m_request->insert("/me/favorites/" + id());
    connect(m_request, SIGNAL(finished()), this, SLOT(onFavouriteRequestFinished()));
    emit statusChanged(status());
}

void DailymotionVideo::unfavourite() {
    if (status() == QDailymotion::ResourcesRequest::Loading) {
        return;
    }
    
    initRequest();
    m_request->del("/me/favorites/" + id());
    connect(m_request, SIGNAL(finished()), this, SLOT(onUnfavouriteRequestFinished()));
    emit statusChanged(status());
}

void DailymotionVideo::initRequest() {
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

void DailymotionVideo::onVideoRequestFinished() {
    if (m_request->status() == QDailymotion::ResourcesRequest::Ready) {
        loadVideo(m_request->result().toMap());
    }
    
    disconnect(m_request, SIGNAL(finished()), this, SLOT(onVideoRequestFinished()));
    emit statusChanged(status());
}

void DailymotionVideo::onFavouriteRequestFinished() {
    if (m_request->status() == QDailymotion::ResourcesRequest::Ready) {
        setFavourite(true);
        emit Dailymotion::instance()->videoFavourited(this);
    }
    
    disconnect(m_request, SIGNAL(finished()), this, SLOT(onFavouriteRequestFinished()));
    emit statusChanged(status());
}

void DailymotionVideo::onUnfavouriteRequestFinished() {
    if (m_request->status() == QDailymotion::ResourcesRequest::Ready) {
        setFavourite(false);
        emit Dailymotion::instance()->videoUnfavourited(this);
    }
    
    disconnect(m_request, SIGNAL(finished()), this, SLOT(onUnfavouriteRequestFinished()));
    emit statusChanged(status());
}

void DailymotionVideo::onVideoUpdated(DailymotionVideo *video) {
    if ((video->id() == id()) && (video != this)) {
        loadVideo(video);
    }
}
