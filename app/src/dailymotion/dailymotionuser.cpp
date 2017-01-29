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

#include "dailymotionuser.h"
#include "dailymotion.h"
#include "logger.h"
#include "resources.h"

DailymotionUser::DailymotionUser(QObject *parent) :
    CTUser(parent),
    m_request(0),
    m_subscribed(false),
    m_subscriberCount(0),
    m_viewCount(0)
{
    setService(Resources::DAILYMOTION);
    connect(Dailymotion::instance(), SIGNAL(userSubscribed(DailymotionUser*)),
            this, SLOT(onUserUpdated(DailymotionUser*)));
    connect(Dailymotion::instance(), SIGNAL(userUnsubscribed(DailymotionUser*)),
            this, SLOT(onUserUpdated(DailymotionUser*)));
}

DailymotionUser::DailymotionUser(const QString &id, QObject *parent) :
    CTUser(parent),
    m_request(0),
    m_subscribed(false),
    m_subscriberCount(0),
    m_viewCount(0)
{
    setService(Resources::DAILYMOTION);
    loadUser(id);
    connect(Dailymotion::instance(), SIGNAL(userSubscribed(DailymotionUser*)),
            this, SLOT(onUserUpdated(DailymotionUser*)));
    connect(Dailymotion::instance(), SIGNAL(userUnsubscribed(DailymotionUser*)),
            this, SLOT(onUserUpdated(DailymotionUser*)));
}

DailymotionUser::DailymotionUser(const QVariantMap &user, QObject *parent) :
    CTUser(parent),
    m_request(0),
    m_subscribed(false),
    m_subscriberCount(0),
    m_viewCount(0)
{
    setService(Resources::DAILYMOTION);
    loadUser(user);
    connect(Dailymotion::instance(), SIGNAL(userSubscribed(DailymotionUser*)),
            this, SLOT(onUserUpdated(DailymotionUser*)));
    connect(Dailymotion::instance(), SIGNAL(userUnsubscribed(DailymotionUser*)),
            this, SLOT(onUserUpdated(DailymotionUser*)));
}

DailymotionUser::DailymotionUser(const DailymotionUser *user, QObject *parent) :
    CTUser(user, parent),
    m_request(0),
    m_bannerUrl(user->bannerUrl()),
    m_largeBannerUrl(user->largeBannerUrl()),
    m_subscribed(user->isSubscribed()),
    m_subscriberCount(user->subscriberCount()),
    m_viewCount(user->viewCount())
{
    connect(Dailymotion::instance(), SIGNAL(userSubscribed(DailymotionUser*)),
            this, SLOT(onUserUpdated(DailymotionUser*)));
    connect(Dailymotion::instance(), SIGNAL(userUnsubscribed(DailymotionUser*)),
            this, SLOT(onUserUpdated(DailymotionUser*)));
}

QUrl DailymotionUser::bannerUrl() const {
    return m_bannerUrl;
}

void DailymotionUser::setBannerUrl(const QUrl &u) {
    if (u != bannerUrl()) {
        m_bannerUrl = u;
        emit bannerUrlChanged();
    }
}

QString DailymotionUser::errorString() const {
    return m_request ? Dailymotion::getErrorString(m_request->result().toMap()) : QString();
}

QUrl DailymotionUser::largeBannerUrl() const {
    return m_largeBannerUrl;
}

void DailymotionUser::setLargeBannerUrl(const QUrl &u) {
    if (u != largeBannerUrl()) {
        m_largeBannerUrl = u;
        emit largeBannerUrlChanged();
    }
}

QDailymotion::ResourcesRequest::Status DailymotionUser::status() const {
    return m_request ? m_request->status() : QDailymotion::ResourcesRequest::Null;
}

bool DailymotionUser::isSubscribed() const {
    return m_subscribed;
}

void DailymotionUser::setSubscribed(bool s) {
    if (s != isSubscribed()) {
        m_subscribed = s;
        emit subscribedChanged();
    }
}

qint64 DailymotionUser::subscriberCount() const {
    return m_subscriberCount;
}

void DailymotionUser::setSubscriberCount(qint64 c) {
    if (c != subscriberCount()) {
        m_subscriberCount = c;
        emit subscriberCountChanged();
    }
}

qint64 DailymotionUser::viewCount() const {
    return m_viewCount;
}

void DailymotionUser::setViewCount(qint64 c) {
    if (c != viewCount()) {
        m_viewCount = c;
        emit viewCountChanged();
    }
}

void DailymotionUser::loadUser(const QString &id) {
    if (status() == QDailymotion::ResourcesRequest::Loading) {
        return;
    }
    
    initRequest();
    m_request->get(id.isEmpty() ? "/me" : "/user/" + id, QVariantMap(), Dailymotion::USER_FIELDS);
    connect(m_request, SIGNAL(finished()), this, SLOT(onUserRequestFinished()));
    emit statusChanged(status());
}

void DailymotionUser::loadUser(const QVariantMap &user) {
    setBannerUrl(user.value("cover_150_url").toString());
    setDescription(user.value("description").toString());
    setId(user.value("id").toString());
    setLargeBannerUrl(user.value("cover_250_url").toString());
    setLargeThumbnailUrl(user.value("avatar_240_url").toString());
    setSubscriberCount(user.value("fans_total").toLongLong());
    setThumbnailUrl(user.value("avatar_80_url").toString());
    setUsername(user.value("screenname").toString());
    setViewCount(user.value("views_total").toLongLong());
}

void DailymotionUser::loadUser(DailymotionUser *user) {
    CTUser::loadUser(user);
    setBannerUrl(user->bannerUrl());
    setLargeBannerUrl(user->largeBannerUrl());
    setSubscribed(user->isSubscribed());
    setSubscriberCount(user->subscriberCount());
    setViewCount(user->viewCount());
}

void DailymotionUser::checkIfSubscribed() {
    if (Dailymotion::subscriptionCache.ids.contains(id())) {
        setSubscribed(true);
        return;
    }
    
    if (status() == QDailymotion::ResourcesRequest::Loading) {
        return;
    }
    
    if (!Dailymotion::subscriptionCache.hasMore) {
        setSubscribed(false);
        return;
    }
    
    Logger::log("DailymotionUser::checkIfSubscribed(). ID: " + id(), Logger::MediumVerbosity);
    initRequest();
    Dailymotion::subscriptionCache.filters["limit"] = 50;
    Dailymotion::subscriptionCache.filters["family_filter"] = false;
    m_request->list("/me/following", Dailymotion::subscriptionCache.filters, QStringList() << "id");
    connect(m_request, SIGNAL(finished()), this, SLOT(onSubscribeCheckRequestFinished()));
    emit statusChanged(status());
}

void DailymotionUser::subscribe() {
    if (status() == QDailymotion::ResourcesRequest::Loading) {
        return;
    }
    
    Logger::log("DailymotionUser::subscribe(). ID: " + id(), Logger::MediumVerbosity);
    initRequest();
    m_request->insert("/me/following/" + id());
    connect(m_request, SIGNAL(finished()), this, SLOT(onSubscribeRequestFinished()));
    emit statusChanged(status());
}

void DailymotionUser::unsubscribe() {
    if (status() == QDailymotion::ResourcesRequest::Loading) {
        return;
    }
    
    Logger::log("DailymotionUser::unsubscribe(). ID: " + id(), Logger::MediumVerbosity);
    initRequest();
    m_request->del("/me/following/" + id());
    connect(m_request, SIGNAL(finished()), this, SLOT(onUnsubscribeRequestFinished()));
    emit statusChanged(status());
}

void DailymotionUser::initRequest() {
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

void DailymotionUser::onUserRequestFinished() {
    if (m_request->status() == QDailymotion::ResourcesRequest::Ready) {
        loadUser(m_request->result().toMap());
    }
    
    disconnect(m_request, SIGNAL(finished()), this, SLOT(onUserRequestFinished()));
    emit statusChanged(status());
}

void DailymotionUser::onSubscribeCheckRequestFinished() {
    if (m_request->status() == QDailymotion::ResourcesRequest::Ready) {
        const QVariantMap result = m_request->result().toMap();
        const QVariantList list = result.value("list").toList();
        Dailymotion::subscriptionCache.hasMore = result.value("has_more").toBool();
        
        const int page = Dailymotion::subscriptionCache.filters.value("page").toInt();
        Dailymotion::subscriptionCache.filters["page"] = (page > 0 ? page + 1 : 2);
        
        foreach (const QVariant &item, list) {
            const QVariantMap map = item.toMap();
            Dailymotion::subscriptionCache.ids.append(map.value("id").toString());
        }
        
        disconnect(m_request, SIGNAL(finished()), this, SLOT(onUserRequestFinished()));
        emit statusChanged(status());
        checkIfSubscribed();
        return;
    }
    
    Logger::log("DailymotionUser::onSubcribeCheckRequestFinished(). Error: " + errorString());
    disconnect(m_request, SIGNAL(finished()), this, SLOT(onUserRequestFinished()));
    emit statusChanged(status());
}

void DailymotionUser::onSubscribeRequestFinished() {
    if (m_request->status() == QDailymotion::ResourcesRequest::Ready) {
        setSubscribed(true);
        setSubscriberCount(subscriberCount() + 1);
        Dailymotion::subscriptionCache.ids.append(id());
        Logger::log("DailymotionUser::onSubscribeRequestFinished(). Subscription added. ID: " + id(),
                    Logger::MediumVerbosity);
        emit Dailymotion::instance()->userSubscribed(this);
    }
    else {
        Logger::log("DailymotionUser::onSubscribeRequestFinished(). Error: " + errorString());
    }
    
    disconnect(m_request, SIGNAL(finished()), this, SLOT(onSubscribeRequestFinished()));
    emit statusChanged(status());
}

void DailymotionUser::onUnsubscribeRequestFinished() {
    if (m_request->status() == QDailymotion::ResourcesRequest::Ready) {
        setSubscribed(false);
        setSubscriberCount(subscriberCount() - 1);
        Dailymotion::subscriptionCache.ids.removeOne(id());
        Logger::log("DailymotionUser::onUnsubscribeRequestFinished(). Subscription removed. ID: " + id(),
                    Logger::MediumVerbosity);
        emit Dailymotion::instance()->userUnsubscribed(this);
    }
    else {
        Logger::log("DailymotionUser::onUnsubscribeRequestFinished(). Error: " + errorString());
    }
    
    disconnect(m_request, SIGNAL(finished()), this, SLOT(onUnsubscribeRequestFinished()));
    emit statusChanged(status());
}

void DailymotionUser::onUserUpdated(DailymotionUser *user) {
    if ((user->id() == id()) && (user != this)) {
        loadUser(user);
    }
}
