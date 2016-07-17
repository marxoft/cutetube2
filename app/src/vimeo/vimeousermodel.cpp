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

#include "vimeousermodel.h"
#include "vimeo.h"

VimeoUserModel::VimeoUserModel(QObject *parent) :
    QAbstractListModel(parent),
    m_request(new QVimeo::ResourcesRequest(this))
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
    m_request->setClientId(Vimeo::clientId());
    m_request->setClientSecret(Vimeo::clientSecret());
    m_request->setAccessToken(Vimeo::accessToken());
    
    connect(m_request, SIGNAL(accessTokenChanged(QString)), Vimeo::instance(), SLOT(setAccessToken(QString)));
    connect(m_request, SIGNAL(finished()), this, SLOT(onRequestFinished()));
}

QString VimeoUserModel::errorString() const {
    return Vimeo::getErrorString(m_request->result().toMap());
}

QVimeo::ResourcesRequest::Status VimeoUserModel::status() const {
    return m_request->status();
}

#if QT_VERSION >=0x050000
QHash<int, QByteArray> VimeoUserModel::roleNames() const {
    return m_roles;
}
#endif

int VimeoUserModel::rowCount(const QModelIndex &) const {
    return m_items.size();
}

bool VimeoUserModel::canFetchMore(const QModelIndex &) const {
    return (status() != QVimeo::ResourcesRequest::Loading) && (m_hasMore);
}

void VimeoUserModel::fetchMore(const QModelIndex &) {
    if (!canFetchMore()) {
        return;
    }
    
    const int page = m_filters.value("page").toInt();
    m_filters["page"] = (page > 0 ? page + 1 : 2);
    m_request->list(m_resourcePath, m_filters);
    emit statusChanged(status());
}

QVariant VimeoUserModel::data(const QModelIndex &index, int role) const {
    if (const VimeoUser *user = get(index.row())) {
        return user->property(m_roles[role]);
    }
    
    return QVariant();
}

QMap<int, QVariant> VimeoUserModel::itemData(const QModelIndex &index) const {
    QMap<int, QVariant> map;
    
    if (const VimeoUser *user = get(index.row())) {
        QHashIterator<int, QByteArray> iterator(m_roles);
        
        while (iterator.hasNext()) {
            iterator.next();
            map[iterator.key()] = user->property(iterator.value());
        }
    }
    
    return map;
}

QVariant VimeoUserModel::data(int row, const QByteArray &role) const {
    if (const VimeoUser *user = get(row)) {
        return user->property(role);
    }
    
    return QVariant();
}

QVariantMap VimeoUserModel::itemData(int row) const {
    QVariantMap map;
    
    if (const VimeoUser *user = get(row)) {
        foreach (const QByteArray &role, m_roles.values()) {
            map[role] = user->property(role);
        }
    }
    
    return map;
}

VimeoUser* VimeoUserModel::get(int row) const {
    if ((row >= 0) && (row < m_items.size())) {
        return m_items.at(row);
    }
    
    return 0;
}

void VimeoUserModel::list(const QString &resourcePath, const QVariantMap &filters) {
    if (status() == QVimeo::ResourcesRequest::Loading) {
        return;
    }
    
    clear();
    m_resourcePath = resourcePath;
    m_filters = filters;
    m_request->list(resourcePath, filters);
    emit statusChanged(status());
    
    disconnect(Vimeo::instance(), 0, this, 0);
    
    if (resourcePath == "/me/following") {
        connect(Vimeo::instance(), SIGNAL(userSubscribed(VimeoUser*)),
                this, SLOT(onUserSubscribed(VimeoUser*)));
        connect(Vimeo::instance(), SIGNAL(userUnsubscribed(VimeoUser*)),
                this, SLOT(onUserUnsubscribed(VimeoUser*)));
    }
}

void VimeoUserModel::clear() {
    if (!m_items.isEmpty()) {
        beginResetModel();
        qDeleteAll(m_items);
        m_items.clear();
        m_hasMore = false;
        endResetModel();
        emit countChanged(rowCount());
    }
}

void VimeoUserModel::cancel() {
    m_request->cancel();
}

void VimeoUserModel::reload() {
    clear();
    m_request->list(m_resourcePath, m_filters);
    emit statusChanged(status());
}

void VimeoUserModel::append(VimeoUser *user) {
    beginInsertRows(QModelIndex(), m_items.size(), m_items.size());
    m_items << user;
    endInsertRows();
}

void VimeoUserModel::insert(int row, VimeoUser *user) {
    if ((row >= 0) && (row < m_items.size())) {
        beginInsertRows(QModelIndex(), row, row);
        m_items.insert(row, user);
        endInsertRows();
    }
    else {
        append(user);
    }
}

void VimeoUserModel::remove(int row) {
    if ((row >= 0) && (row < m_items.size())) {
        beginRemoveRows(QModelIndex(), row, row);
        m_items.takeAt(row)->deleteLater();
        endRemoveRows();
    }
}

void VimeoUserModel::onRequestFinished() {
    if (m_request->status() == QVimeo::ResourcesRequest::Ready) {
        const QVariantMap result = m_request->result().toMap();
        
        if (!result.isEmpty()) {
            m_hasMore = !result.value("paging").toMap().value("next").isNull();
            const QVariantList list = result.value("data").toList();

            beginInsertRows(QModelIndex(), m_items.size(), m_items.size() + list.size() - 1);
    
            foreach (const QVariant &item, list) {
                m_items << new VimeoUser(item.toMap(), this);
            }

            endInsertRows();
            emit countChanged(rowCount());
        }
    }
    
    emit statusChanged(status());
}

void VimeoUserModel::onUserSubscribed(VimeoUser *user) {
    insert(0, new VimeoUser(user, this));
}

void VimeoUserModel::onUserUnsubscribed(VimeoUser *user) {
    const QModelIndexList list = match(index(0), IdRole, user->id(), 1, Qt::MatchExactly);
    
    if (!list.isEmpty()) {
        remove(list.first().row());
    }
}
