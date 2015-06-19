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

#include "pluginusermodel.h"
#include "resources.h"

PluginUserModel::PluginUserModel(QObject *parent) :
    QAbstractListModel(parent),
    m_request(new ResourcesRequest(this))
{
    m_roles[DescriptionRole] = "description";
    m_roles[IdRole] = "id";
    m_roles[LargeThumbnailUrlRole] = "largeThumbnailUrl";
    m_roles[ServiceRole] = "service";
    m_roles[ThumbnailUrlRole] = "thumbnailUrl";
    m_roles[UsernameRole] = "username";
#if QT_VERSION < 0x050000
    setRoleNames(m_roles);
#endif
    connect(m_request, SIGNAL(serviceChanged()), this, SIGNAL(serviceChanged()));
    connect(m_request, SIGNAL(finished()), this, SLOT(onRequestFinished()));
}

QString PluginUserModel::service() const {
    return m_request->service();
}

void PluginUserModel::setService(const QString &s) {
    m_request->setService(s);
}

QString PluginUserModel::errorString() const {
    return m_request->errorString();
}

ResourcesRequest::Status PluginUserModel::status() const {
    return m_request->status();
}

#if QT_VERSION >=0x050000
QHash<int, QByteArray> PluginUserModel::roleNames() const {
    return m_roles;
}
#endif

int PluginUserModel::rowCount(const QModelIndex &) const {
    return m_items.size();
}

bool PluginUserModel::canFetchMore(const QModelIndex &) const {
    return (status() != ResourcesRequest::Loading) && (!m_next.isEmpty());
}

void PluginUserModel::fetchMore(const QModelIndex &) {
    if (!canFetchMore()) {
        return;
    }
    
    m_request->list(Resources::USER, m_next);
    emit statusChanged(status());
}

QVariant PluginUserModel::data(const QModelIndex &index, int role) const {
    if (PluginUser *user = get(index.row())) {
        return user->property(m_roles[role]);
    }
    
    return QVariant();
}

QMap<int, QVariant> PluginUserModel::itemData(const QModelIndex &index) const {
    QMap<int, QVariant> map;
    
    if (PluginUser *user = get(index.row())) {
        QHashIterator<int, QByteArray> iterator(m_roles);
        
        while (iterator.hasNext()) {
            iterator.next();
            map[iterator.key()] = user->property(iterator.value());
        }
    }
    
    return map;
}

QVariant PluginUserModel::data(int row, const QByteArray &role) const {
    if (PluginUser *user = get(row)) {
        return user->property(role);
    }
    
    return QVariant();
}

QVariantMap PluginUserModel::itemData(int row) const {
    QVariantMap map;
    
    if (PluginUser *user = get(row)) {
        foreach (QByteArray role, m_roles.values()) {
            map[role] = user->property(role);
        }
    }
    
    return map;
}

PluginUser* PluginUserModel::get(int row) const {
    if ((row >= 0) && (row < m_items.size())) {
        return m_items.at(row);
    }
    
    return 0;
}

void PluginUserModel::list(const QString &id) {
    if (status() == ResourcesRequest::Loading) {
        return;
    }
    
    clear();
    m_id = id;
    m_query = QString();
    m_request->list(Resources::USER, id);
    emit statusChanged(status());
}

void PluginUserModel::search(const QString &query, const QString &order) {
    if (status() == ResourcesRequest::Loading) {
        return;
    }
    
    clear();
    m_id = QString();
    m_query = query;
    m_order = order;
    m_request->search(Resources::USER, query, order);
    emit statusChanged(status());
}

void PluginUserModel::clear() {
    if (!m_items.isEmpty()) {
        beginResetModel();
        qDeleteAll(m_items);
        m_items.clear();
        m_next = QString();
        endResetModel();
        emit countChanged(rowCount());
    }
}

void PluginUserModel::cancel() {
    m_request->cancel();
}

void PluginUserModel::reload() {
    clear();
    
    if (m_query.isEmpty()) {
        m_request->list(Resources::USER, m_id);
    }
    else {
        m_request->search(Resources::USER, m_query, m_order);
    }
    
    emit statusChanged(status());
}

void PluginUserModel::append(PluginUser *user) {
    beginInsertRows(QModelIndex(), m_items.size(), m_items.size());
    m_items << user;
    endInsertRows();
}

void PluginUserModel::insert(int row, PluginUser *user) {
    if ((row >= 0) && (row < m_items.size())) {
        beginInsertRows(QModelIndex(), row, row);
        m_items.insert(row, user);
        endInsertRows();
    }
    else {
        append(user);
    }
}

void PluginUserModel::remove(int row) {
    if ((row >= 0) && (row < m_items.size())) {
        beginRemoveRows(QModelIndex(), row, row);
        m_items.takeAt(row)->deleteLater();
        endRemoveRows();
    }
}

void PluginUserModel::onRequestFinished() {
    if (m_request->status() == ResourcesRequest::Ready) {
        QVariantMap result = m_request->result().toMap();
        
        if (!result.isEmpty()) {
            m_next = result.value("next").toString();
            QVariantList list = result.value("items").toList();

            beginInsertRows(QModelIndex(), m_items.size(), m_items.size() + list.size() - 1);
    
            foreach (QVariant item, list) {
                m_items << new PluginUser(service(), item.toMap(), this);
            }

            endInsertRows();
            emit countChanged(rowCount());
                
        }
    }
    
    emit statusChanged(status());
}
