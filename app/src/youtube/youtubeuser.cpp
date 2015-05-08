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

#include "youtubeuser.h"
#include "resources.h"
#include "youtube.h"
#ifdef CUTETUBE_DEBUG
#include <QDebug>
#endif

YouTubeUser::YouTubeUser(QObject *parent) :
    User(parent),
    m_request(0),
    m_subscribed(false),
    m_subscriberCount(0),
    m_viewCount(0)
{
    setService(Resources::YOUTUBE);
    connect(YouTube::instance(), SIGNAL(userSubscribed(const YouTubeUser*)),
            this, SLOT(onUserUpdated(const YouTubeUser*)));
    connect(YouTube::instance(), SIGNAL(userUnsubscribed(const YouTubeUser*)),
            this, SLOT(onUserUpdated(const YouTubeUser*)));
}

YouTubeUser::YouTubeUser(const QString &id, QObject *parent) :
    User(parent),
    m_request(0),
    m_subscribed(false),
    m_subscriberCount(0),
    m_viewCount(0)
{
    setService(Resources::YOUTUBE);
    loadUser(id);
    connect(YouTube::instance(), SIGNAL(userSubscribed(const YouTubeUser*)),
            this, SLOT(onUserUpdated(const YouTubeUser*)));
    connect(YouTube::instance(), SIGNAL(userUnsubscribed(const YouTubeUser*)),
            this, SLOT(onUserUpdated(const YouTubeUser*)));
}

YouTubeUser::YouTubeUser(const QVariantMap &user, QObject *parent) :
    User(parent),
    m_request(0),
    m_subscribed(false),
    m_subscriberCount(0),
    m_viewCount(0)
{
    setService(Resources::YOUTUBE);
    loadUser(user);
    connect(YouTube::instance(), SIGNAL(userSubscribed(const YouTubeUser*)),
            this, SLOT(onUserUpdated(const YouTubeUser*)));
    connect(YouTube::instance(), SIGNAL(userUnsubscribed(const YouTubeUser*)),
            this, SLOT(onUserUpdated(const YouTubeUser*)));
}

YouTubeUser::YouTubeUser(const YouTubeUser *user, QObject *parent) :
    User(user, parent),
    m_request(0),
    m_bannerUrl(user->bannerUrl()),
    m_largeBannerUrl(user->largeBannerUrl()),
    m_relatedPlaylists(user->relatedPlaylists()),
    m_subscribed(user->isSubscribed()),
    m_subscriptionId(user->subscriptionId()),
    m_subscriberCount(user->subscriberCount()),
    m_viewCount(user->viewCount())
{
    connect(YouTube::instance(), SIGNAL(userSubscribed(const YouTubeUser*)),
            this, SLOT(onUserUpdated(const YouTubeUser*)));
    connect(YouTube::instance(), SIGNAL(userUnsubscribed(const YouTubeUser*)),
            this, SLOT(onUserUpdated(const YouTubeUser*)));
}

QUrl YouTubeUser::bannerUrl() const {
    return m_bannerUrl;
}

void YouTubeUser::setBannerUrl(const QUrl &u) {
    if (u != bannerUrl()) {
        m_bannerUrl = u;
        emit bannerUrlChanged();
    }
}

QString YouTubeUser::errorString() const {
    return m_request ? YouTube::getErrorString(m_request->result().toMap()) : QString();
}

QUrl YouTubeUser::largeBannerUrl() const {
    return m_largeBannerUrl;
}

void YouTubeUser::setLargeBannerUrl(const QUrl &u) {
    if (u != largeBannerUrl()) {
        m_largeBannerUrl = u;
        emit largeBannerUrlChanged();
    }
}

QVariantMap YouTubeUser::relatedPlaylists() const {
    return m_relatedPlaylists;
}

void YouTubeUser::setRelatedPlaylists(const QVariantMap &p) {
    m_relatedPlaylists = p;
    emit relatedPlaylistsChanged();
}

QYouTube::ResourcesRequest::Status YouTubeUser::status() const {
    return m_request ? m_request->status() : QYouTube::ResourcesRequest::Null;
}

bool YouTubeUser::isSubscribed() const {
    return m_subscribed;
}

void YouTubeUser::setSubscribed(bool s) {
    if (s != isSubscribed()) {
        m_subscribed = s;
        emit subscribedChanged();
    }
}

QString YouTubeUser::subscriptionId() const {
    return m_subscriptionId;
}

void YouTubeUser::setSubscriptionId(const QString &i) {
    if (i != subscriptionId()) {
        m_subscriptionId = i;
        emit subscriptionIdChanged();
    }
}

qint64 YouTubeUser::subscriberCount() const {
    return m_subscriberCount;
}

void YouTubeUser::setSubscriberCount(qint64 c) {
    if (c != subscriberCount()) {
        m_subscriberCount = c;
        emit subscriberCountChanged();
    }
}

qint64 YouTubeUser::viewCount() const {
    return m_viewCount;
}

void YouTubeUser::setViewCount(qint64 c) {
    if (c != viewCount()) {
        m_viewCount = c;
        emit viewCountChanged();
    }
}

void YouTubeUser::loadUser(const QString &id) {
    if (status() == QYouTube::ResourcesRequest::Loading) {
        return;
    }
    
    initRequest();
    
    QVariantMap filters;
    
    if (id.isEmpty()) {
        filters["mine"] = true;
    }
    else {
        filters["id"] = id;
    }
    
    m_request->list("/channels", QStringList() << "snippet" << "contentDetails" << "brandingSettings" << "statistics",
                    filters);
    connect(m_request, SIGNAL(finished()), this, SLOT(onUserRequestFinished()));
    emit statusChanged(status());
}

void YouTubeUser::loadUser(const QVariantMap &user) {
    QVariantMap brandingSettings = user.value("brandingSettings").toMap();
    QVariantMap banners = brandingSettings.value("image").toMap();
    QVariantMap channel = brandingSettings.value("channel").toMap();
    QVariantMap statistics = user.value("statistics").toMap();
    QVariantMap thumbnails = user.value("snippet").toMap().value("thumbnails").toMap();
    
    setBannerUrl(banners.value("bannerMobileImageUrl").toString());
    setDescription(channel.value("description").toString());
    setId(YouTube::getUserId(user));
    setLargeBannerUrl(banners.value("bannerImageUrl").toString());
    setLargeThumbnailUrl(thumbnails.value("high").toMap().value("url").toString());
    setRelatedPlaylists(user.value("contentDetails").toMap().value("relatedPlaylists").toMap());
    setSubscribed(user.value("kind") == "youtube#subscriptionItem");
    setSubscriberCount(statistics.value("subscriberCount").toLongLong());
    setThumbnailUrl(thumbnails.value("default").toMap().value("url").toString());
    setUsername(channel.value("title").toString());
    setViewCount(statistics.value("viewCount").toLongLong());
}

void YouTubeUser::loadUser(const YouTubeUser *user) {
    User::loadUser(user);
    setBannerUrl(user->bannerUrl());
    setLargeBannerUrl(user->largeBannerUrl());
    setRelatedPlaylists(user->relatedPlaylists());
    setSubscribed(user->isSubscribed());
    setSubscriptionId(user->subscriptionId());
    setSubscriberCount(user->subscriberCount());
    setUsername(user->username());
    setViewCount(user->viewCount());
}

void YouTubeUser::checkIfSubscribed() {
    if (YouTube::subscriptionCache.ids.contains(id())) {
        setSubscribed(true);
        setSubscriptionId(YouTube::subscriptionCache.ids.value(id()));
        return;
    }
    
    if (status() == QYouTube::ResourcesRequest::Loading) {
        return;
    }
    
    if ((!YouTube::subscriptionCache.ids.isEmpty()) && (YouTube::subscriptionCache.nextPageToken.isEmpty())) {
#ifdef CUTETUBE_DEBUG
        qDebug() << "YouTubeUser::checkIfSubscribed" << id() << "not found";
#endif
        setSubscribed(false);
        setSubscriptionId(QString());
        return;
    }
    
    initRequest();
    
    QVariantMap filters;
    filters["mine"] = true;
    
    QVariantMap params;
    params["maxResults"] = 50;
    
    if (!YouTube::subscriptionCache.nextPageToken.isEmpty()) {
        params["nextPageToken"] = YouTube::subscriptionCache.nextPageToken;
    }
    
    m_request->list("/subscriptions", QStringList() << "snippet", filters, params);
    connect(m_request, SIGNAL(finished()), this, SLOT(onSubscribeCheckRequestFinished()));
    emit statusChanged(status());
}

void YouTubeUser::subscribe() {
    if (status() == QYouTube::ResourcesRequest::Loading) {
        return;
    }
    
    initRequest();
        
    QVariantMap resource;
    QVariantMap snippet;
    QVariantMap resourceId;
    resourceId["kind"] = "youtube#channel";
    resourceId["channelId"] = id();
    snippet["resourceId"] = resourceId;
    resource["snippet"] = snippet;
    
    m_request->insert(resource, "/subscriptions", QStringList() << "snippet");
    connect(m_request, SIGNAL(finished()), this, SLOT(onSubscribeRequestFinished()));
    emit statusChanged(status());
}

void YouTubeUser::unsubscribe() {
    if (status() == QYouTube::ResourcesRequest::Loading) {
        return;
    }
    
    initRequest();
    m_request->del(subscriptionId(), "/subscriptions");
    connect(m_request, SIGNAL(finished()), this, SLOT(onUnsubscribeRequestFinished()));
    emit statusChanged(status());
}

void YouTubeUser::initRequest() {
    if (!m_request) {
        m_request = new QYouTube::ResourcesRequest(this);
        m_request->setApiKey(YouTube::instance()->apiKey());
        m_request->setClientId(YouTube::instance()->clientId());
        m_request->setClientSecret(YouTube::instance()->clientSecret());
        m_request->setAccessToken(YouTube::instance()->accessToken());
        m_request->setRefreshToken(YouTube::instance()->refreshToken());
    
        connect(m_request, SIGNAL(accessTokenChanged(QString)), YouTube::instance(), SLOT(setAccessToken(QString)));
        connect(m_request, SIGNAL(refreshTokenChanged(QString)), YouTube::instance(), SLOT(setRefreshToken(QString)));
    }
}

void YouTubeUser::onUserRequestFinished() {
    if (m_request->status() == QYouTube::ResourcesRequest::Ready) {
        QVariantList list = m_request->result().toMap().value("items").toList();
        
        if (!list.isEmpty()) {
            loadUser(list.first().toMap());
        }
    }
    
    disconnect(m_request, SIGNAL(finished()), this, SLOT(onUserRequestFinished()));
    emit statusChanged(status());
}

void YouTubeUser::onSubscribeCheckRequestFinished() {
    if (m_request->status() == QYouTube::ResourcesRequest::Ready) {
        QVariantMap result = m_request->result().toMap();
        QVariantList items = result.value("items").toList();
        YouTube::subscriptionCache.nextPageToken = result.value("nextPageToken").toString();
        
        foreach(QVariant item, items) {
            QVariantMap map = item.toMap();
            YouTube::subscriptionCache.ids.insert(map.value("snippet").toMap().value("resourceId").toMap()
                                        .value("channelId").toString(), map.value("id").toString());
        }
#ifdef CUTETUBE_DEBUG
        qDebug() << "YouTubeUser::onSubscribeCheckRequestFinished OK" << YouTube::subscriptionCache.ids;
#endif
        disconnect(m_request, SIGNAL(finished()), this, SLOT(onUserRequestFinished()));
        emit statusChanged(status());
        checkIfSubscribed();
        return;
    }
    
    disconnect(m_request, SIGNAL(finished()), this, SLOT(onUserRequestFinished()));
    emit statusChanged(status());
}

void YouTubeUser::onSubscribeRequestFinished() {
    if (m_request->status() == QYouTube::ResourcesRequest::Ready) {
        setSubscribed(true);
        setSubscriberCount(subscriberCount() + 1);
        setSubscriptionId(m_request->result().toMap().value("id").toString());
        YouTube::subscriptionCache.ids.insert(id(), subscriptionId());
        emit YouTube::instance()->userSubscribed(this);
#ifdef CUTETUBE_DEBUG
        qDebug() << "YouTubeUser::onSubscribeRequestFinished OK" << id();
#endif
    }
    
    disconnect(m_request, SIGNAL(finished()), this, SLOT(onSubscribeRequestFinished()));
    emit statusChanged(status());
}

void YouTubeUser::onUnsubscribeRequestFinished() {
    if (m_request->status() == QYouTube::ResourcesRequest::Ready) {
        setSubscribed(false);
        setSubscriberCount(subscriberCount() - 1);
        setSubscriptionId(QString());
        YouTube::subscriptionCache.ids.remove(id());
        emit YouTube::instance()->userUnsubscribed(this);
#ifdef CUTETUBE_DEBUG
        qDebug() << "YouTubeUser::onUnsubscribeRequestFinished OK" << id();
#endif
    }
    
    disconnect(m_request, SIGNAL(finished()), this, SLOT(onUnsubscribeRequestFinished()));
    emit statusChanged(status());
}

void YouTubeUser::onUserUpdated(const YouTubeUser *user) {
    if ((user->id() == id()) && (user != this)) {
        loadUser(user);
    }
}
