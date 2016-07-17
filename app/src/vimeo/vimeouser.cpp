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

#include "vimeouser.h"
#include "resources.h"
#include "vimeo.h"

VimeoUser::VimeoUser(QObject *parent) :
    CTUser(parent),
    m_request(0),
    m_subscribed(false),
    m_subscriberCount(0)
{
    setService(Resources::VIMEO);
    connect(Vimeo::instance(), SIGNAL(userSubscribed(VimeoUser*)),
            this, SLOT(onUserUpdated(VimeoUser*)));
    connect(Vimeo::instance(), SIGNAL(userUnsubscribed(VimeoUser*)),
            this, SLOT(onUserUpdated(VimeoUser*)));
}

VimeoUser::VimeoUser(const QString &id, QObject *parent) :
    CTUser(parent),
    m_request(0),
    m_subscribed(false),
    m_subscriberCount(0)
{
    setService(Resources::VIMEO);
    loadUser(id);
    connect(Vimeo::instance(), SIGNAL(userSubscribed(VimeoUser*)),
            this, SLOT(onUserUpdated(VimeoUser*)));
    connect(Vimeo::instance(), SIGNAL(userUnsubscribed(VimeoUser*)),
            this, SLOT(onUserUpdated(VimeoUser*)));
}

VimeoUser::VimeoUser(const QVariantMap &user, QObject *parent) :
    CTUser(parent),
    m_request(0),
    m_subscribed(false),
    m_subscriberCount(0)
{
    setService(Resources::VIMEO);
    loadUser(user);
    connect(Vimeo::instance(), SIGNAL(userSubscribed(VimeoUser*)),
            this, SLOT(onUserUpdated(VimeoUser*)));
    connect(Vimeo::instance(), SIGNAL(userUnsubscribed(VimeoUser*)),
            this, SLOT(onUserUpdated(VimeoUser*)));
}

VimeoUser::VimeoUser(const VimeoUser *user, QObject *parent) :
    CTUser(user, parent),
    m_request(0),
    m_subscribed(user->isSubscribed()),
    m_subscriberCount(user->subscriberCount())
{
    connect(Vimeo::instance(), SIGNAL(userSubscribed(VimeoUser*)),
            this, SLOT(onUserUpdated(VimeoUser*)));
    connect(Vimeo::instance(), SIGNAL(userUnsubscribed(VimeoUser*)),
            this, SLOT(onUserUpdated(VimeoUser*)));
}

QString VimeoUser::errorString() const {
    return m_request ? Vimeo::getErrorString(m_request->result().toMap()) : QString();
}

QVimeo::ResourcesRequest::Status VimeoUser::status() const {
    return m_request ? m_request->status() : QVimeo::ResourcesRequest::Null;
}

bool VimeoUser::isSubscribed() const {
    return m_subscribed;
}

void VimeoUser::setSubscribed(bool s) {
    if (s != isSubscribed()) {
        m_subscribed = s;
        emit subscribedChanged();
    }
}

qint64 VimeoUser::subscriberCount() const {
    return m_subscriberCount;
}

void VimeoUser::setSubscriberCount(qint64 c) {
    if (c != subscriberCount()) {
        m_subscriberCount = c;
        emit subscriberCountChanged();
    }
}

void VimeoUser::loadUser(const QString &id) {
    if (status() == QVimeo::ResourcesRequest::Loading) {
        return;
    }
    
    initRequest();
    m_request->get(id.isEmpty() ? "/me" : "/users/" + id);
    connect(m_request, SIGNAL(finished()), this, SLOT(onUserRequestFinished()));
    emit statusChanged(status());
}

void VimeoUser::loadUser(const QVariantMap &user) {
    setDescription(user.value("bio").toString());
    setId(user.value("uri").toString().section('/', -1));
    setLargeThumbnailUrl(QString("https://i.vimeocdn.com/portrait/%1_300x300.jpg").arg(id()));
    setSubscriberCount(user.value("metadata").toMap().value("connections").toMap().value("followers").toMap()
                       .value("total").toLongLong());
    setThumbnailUrl(QString("https://i.vimeocdn.com/portrait/%1_75x75.jpg").arg(id()));
    setUsername(user.value("name").toString());
}

void VimeoUser::loadUser(VimeoUser *user) {
    CTUser::loadUser(user);
    setSubscribed(user->isSubscribed());
    setSubscriberCount(user->subscriberCount());
}

void VimeoUser::checkIfSubscribed() {
    if (Vimeo::subscriptionCache.ids.contains(id())) {
        setSubscribed(true);
        return;
    }
    
    if (status() == QVimeo::ResourcesRequest::Loading) {
        return;
    }
    
    if (!Vimeo::subscriptionCache.hasMore) {
        setSubscribed(false);
        return;
    }
    
    initRequest();
    Vimeo::subscriptionCache.filters["per_page"] = 50;
    m_request->list("/me/following", Vimeo::subscriptionCache.filters);
    connect(m_request, SIGNAL(finished()), this, SLOT(onSubscribeCheckRequestFinished()));
    emit statusChanged(status());
}

void VimeoUser::subscribe() {
    if (status() == QVimeo::ResourcesRequest::Loading) {
        return;
    }
    
    initRequest();
    m_request->insert("/me/following/" + id());
    connect(m_request, SIGNAL(finished()), this, SLOT(onSubscribeRequestFinished()));
    emit statusChanged(status());
}

void VimeoUser::unsubscribe() {
    if (status() == QVimeo::ResourcesRequest::Loading) {
        return;
    }
    
    initRequest();
    m_request->del("/me/following/" + id());
    connect(m_request, SIGNAL(finished()), this, SLOT(onUnsubscribeRequestFinished()));
    emit statusChanged(status());
}

void VimeoUser::initRequest() {
    if (!m_request) {
        m_request = new QVimeo::ResourcesRequest(this);
        m_request->setClientId(Vimeo::clientId());
        m_request->setClientSecret(Vimeo::clientSecret());
        m_request->setAccessToken(Vimeo::accessToken());
    
        connect(m_request, SIGNAL(accessTokenChanged(QString)), Vimeo::instance(), SLOT(setAccessToken(QString)));
    }
}

void VimeoUser::onUserRequestFinished() {
    if (m_request->status() == QVimeo::ResourcesRequest::Ready) {
        loadUser(m_request->result().toMap());
    }
    
    disconnect(m_request, SIGNAL(finished()), this, SLOT(onUserRequestFinished()));
    emit statusChanged(status());
}

void VimeoUser::onSubscribeCheckRequestFinished() {
    if (m_request->status() == QVimeo::ResourcesRequest::Ready) {
        const QVariantMap result = m_request->result().toMap();
        const QVariantList list = result.value("data").toList();
        Vimeo::subscriptionCache.hasMore = !result.value("paging").toMap().value("next").isNull();
        
        const int page = Vimeo::subscriptionCache.filters.value("page").toInt();
        Vimeo::subscriptionCache.filters["page"] = (page > 0 ? page + 1 : 2);
        
        foreach (const QVariant &item, list) {
            const QVariantMap map = item.toMap();
            Vimeo::subscriptionCache.ids.append(map.value("id").toString());
        }
        
        disconnect(m_request, SIGNAL(finished()), this, SLOT(onUserRequestFinished()));
        emit statusChanged(status());
        checkIfSubscribed();
        return;
    }
    
    disconnect(m_request, SIGNAL(finished()), this, SLOT(onUserRequestFinished()));
    emit statusChanged(status());
}

void VimeoUser::onSubscribeRequestFinished() {
    if (m_request->status() == QVimeo::ResourcesRequest::Ready) {
        setSubscribed(true);
        setSubscriberCount(subscriberCount() + 1);
        Vimeo::subscriptionCache.ids.append(id());
        emit Vimeo::instance()->userSubscribed(this);
    }
    
    disconnect(m_request, SIGNAL(finished()), this, SLOT(onSubscribeRequestFinished()));
    emit statusChanged(status());
}

void VimeoUser::onUnsubscribeRequestFinished() {
    if (m_request->status() == QVimeo::ResourcesRequest::Ready) {
        setSubscribed(false);
        setSubscriberCount(subscriberCount() - 1);
        Vimeo::subscriptionCache.ids.removeOne(id());
        emit Vimeo::instance()->userUnsubscribed(this);
    }
    
    disconnect(m_request, SIGNAL(finished()), this, SLOT(onUnsubscribeRequestFinished()));
    emit statusChanged(status());
}

void VimeoUser::onUserUpdated(VimeoUser *user) {
    if ((user->id() == id()) && (user != this)) {
        loadUser(user);
    }
}
