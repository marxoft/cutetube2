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

#include "dailymotionusermodel.h"
#include "dailymotion.h"
#ifdef CUTETUBE_DEBUG
#include <QDebug>
#endif

DailymotionUserModel::DailymotionUserModel(QObject *parent) :
    QAbstractListModel(parent),
    m_request(new QDailymotion::ResourcesRequest(this)),
    m_hasMore(false)
{
    m_roles[BannerUrlRole] = "bannerUrl";
    m_roles[DescriptionRole] = "description";
    m_roles[IdRole] = "id";
    m_roles[LargeBannerUrlRole] = "largeBannerUrl";
    m_roles[LargeThumbnailUrlRole] = "largeThumbnailUrl";
    m_roles[SubscribedRole] = "subscribed";
    m_roles[SubscriberCountRole] = "subscriberCount";
    m_roles[ThumbnailUrlRole] = "thumbnailUrl";
    m_roles[UsernameRole] = "username";
#if QT_VERSION < 0x050000
    setRoleNames(m_roles);
#endif
    m_request->setClientId(Dailymotion::instance()->clientId());
    m_request->setClientSecret(Dailymotion::instance()->clientSecret());
    m_request->setAccessToken(Dailymotion::instance()->accessToken());
    m_request->setRefreshToken(Dailymotion::instance()->refreshToken());
    
    connect(m_request, SIGNAL(accessTokenChanged(QString)), Dailymotion::instance(), SLOT(setAccessToken(QString)));
    connect(m_request, SIGNAL(refreshTokenChanged(QString)), Dailymotion::instance(), SLOT(setRefreshToken(QString)));
    connect(m_request, SIGNAL(finished()), this, SLOT(onRequestFinished()));
}

QString DailymotionUserModel::errorString() const {
    return Dailymotion::getErrorString(m_request->result().toMap());
}

QDailymotion::ResourcesRequest::Status DailymotionUserModel::status() const {
    return m_request->status();
}

#if QT_VERSION >=0x050000
QHash<int, QByteArray> DailymotionUserModel::roleNames() const {
    return m_roles;
}
#endif

int DailymotionUserModel::rowCount(const QModelIndex &) const {
    return m_items.size();
}

bool DailymotionUserModel::canFetchMore(const QModelIndex &) const {
    return (status() != QDailymotion::ResourcesRequest::Loading) && (m_hasMore);
}

void DailymotionUserModel::fetchMore(const QModelIndex &) {
    if (!canFetchMore()) {
        return;
    }
    
    const int page = m_filters.value("page").toInt();
    m_filters["page"] = (page > 0 ? page + 1 : 2);
    m_request->list(m_resourcePath, m_filters, Dailymotion::USER_FIELDS);
    emit statusChanged(status());
}

QVariant DailymotionUserModel::data(const QModelIndex &index, int role) const {
    if (DailymotionUser *user = get(index.row())) {
        return user->property(m_roles[role]);
    }
    
    return QVariant();
}

QMap<int, QVariant> DailymotionUserModel::itemData(const QModelIndex &index) const {
    QMap<int, QVariant> map;
    
    if (DailymotionUser *user = get(index.row())) {
        QHashIterator<int, QByteArray> iterator(m_roles);
        
        while (iterator.hasNext()) {
            iterator.next();
            map[iterator.key()] = user->property(iterator.value());
        }
    }
    
    return map;
}

QVariant DailymotionUserModel::data(int row, const QByteArray &role) const {
    if (DailymotionUser *user = get(row)) {
        return user->property(role);
    }
    
    return QVariant();
}

QVariantMap DailymotionUserModel::itemData(int row) const {
    QVariantMap map;
    
    if (DailymotionUser *user = get(row)) {
        foreach (QByteArray role, m_roles.values()) {
            map[role] = user->property(role);
        }
    }
    
    return map;
}

DailymotionUser* DailymotionUserModel::get(int row) const {
    if ((row >= 0) && (row < m_items.size())) {
        return m_items.at(row);
    }
    
    return 0;
}

void DailymotionUserModel::list(const QString &resourcePath, const QVariantMap &filters) {
    if (status() == QDailymotion::ResourcesRequest::Loading) {
        return;
    }
    
    clear();
    m_resourcePath = resourcePath;
    m_filters = filters;
    m_request->list(resourcePath, filters, Dailymotion::USER_FIELDS);
    emit statusChanged(status());
    
    disconnect(Dailymotion::instance(), 0, this, 0);
    
    if (resourcePath == "/me/following") {
        connect(Dailymotion::instance(), SIGNAL(userSubscribed(const DailymotionUser*)),
                this, SLOT(onUserSubscribed(const DailymotionUser*)));
        connect(Dailymotion::instance(), SIGNAL(userUnsubscribed(const DailymotionUser*)),
                this, SLOT(onUserUnsubscribed(const DailymotionUser*)));
    }
}

void DailymotionUserModel::clear() {
    if (!m_items.isEmpty()) {
        beginResetModel();
        m_items.clear();
        endResetModel();
        emit countChanged(rowCount());
    }
}

void DailymotionUserModel::cancel() {
    m_request->cancel();
}

void DailymotionUserModel::reload() {
    clear();
    m_request->list(m_resourcePath,  m_filters, Dailymotion::USER_FIELDS);
    emit statusChanged(status());
}

void DailymotionUserModel::append(DailymotionUser *user) {
    beginInsertRows(QModelIndex(), m_items.size(), m_items.size());
    m_items << user;
    endInsertRows();
}

void DailymotionUserModel::insert(int row, DailymotionUser *user) {
    if ((row >= 0) && (row < m_items.size())) {
        beginInsertRows(QModelIndex(), row, row);
        m_items.insert(row, user);
        endInsertRows();
    }
    else {
        append(user);
    }
}

void DailymotionUserModel::remove(int row) {
    if ((row >= 0) && (row < m_items.size())) {
        beginRemoveRows(QModelIndex(), row, row);
        m_items.takeAt(row)->deleteLater();
        endRemoveRows();
    }
}

void DailymotionUserModel::onRequestFinished() {
    if (m_request->status() == QDailymotion::ResourcesRequest::Ready) {
        QVariantMap result = m_request->result().toMap();
        
        if (!result.isEmpty()) {
            m_hasMore = result.value("has_more").toBool();
            QVariantList list = result.value("list").toList();

            beginInsertRows(QModelIndex(), m_items.size(), m_items.size() + list.size() - 1);
    
            foreach (QVariant item, list) {
                m_items << new DailymotionUser(item.toMap(), this);
            }

            endInsertRows();
            emit countChanged(rowCount());
        }
    }
    
    emit statusChanged(status());
}

void DailymotionUserModel::onUserSubscribed(const DailymotionUser *user) {
    insert(0, new DailymotionUser(user, this));
#ifdef CUTETUBE_DEBUG
    qDebug() << "DailymotionUserModel::onUserSubscribed" << user->id();
#endif
}

void DailymotionUserModel::onUserUnsubscribed(const DailymotionUser *user) {
    QModelIndexList list = match(index(0), IdRole, user->id(), 1, Qt::MatchExactly);
    
    if (!list.isEmpty()) {
        remove(list.first().row());
    }
#ifdef CUTETUBE_DEBUG
    qDebug() << "DailymotionUserModel::onUserUnsubscribed" << user->id();
#endif
}
