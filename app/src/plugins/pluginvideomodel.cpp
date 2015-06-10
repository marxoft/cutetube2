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

#include "pluginvideomodel.h"
#include "resources.h"

PluginVideoModel::PluginVideoModel(QObject *parent) :
    QAbstractListModel(parent),
    m_request(new ResourcesRequest(this))
{
    m_roles[DateRole] = "date";
    m_roles[DescriptionRole] = "description";
    m_roles[DownloadableRole] = "downloadable";
    m_roles[DurationRole] = "duration";
    m_roles[IdRole] = "id";
    m_roles[LargeThumbnailUrlRole] = "largeThumbnailUrl";
    m_roles[ServiceRole] = "service";
    m_roles[StreamUrlRole] = "streamUrl";
    m_roles[ThumbnailUrlRole] = "thumbnailUrl";
    m_roles[TitleRole] = "title";
    m_roles[UrlRole] = "url";
    m_roles[UserIdRole] = "userId";
    m_roles[UsernameRole] = "username";
    m_roles[ViewCountRole] = "viewCount";
#if QT_VERSION < 0x050000
    setRoleNames(m_roles);
#endif
    connect(m_request, SIGNAL(serviceChanged()), this, SIGNAL(serviceChanged()));
    connect(m_request, SIGNAL(finished()), this, SLOT(onRequestFinished()));
}

QString PluginVideoModel::service() const {
    return m_request->service();
}

void PluginVideoModel::setService(const QString &s) {
    m_request->setService(s);
}

QString PluginVideoModel::errorString() const {
    return m_request->errorString();
}

ResourcesRequest::Status PluginVideoModel::status() const {
    return m_request->status();
}

#if QT_VERSION >=0x050000
QHash<int, QByteArray> PluginVideoModel::roleNames() const {
    return m_roles;
}
#endif

int PluginVideoModel::rowCount(const QModelIndex &) const {
    return m_items.size();
}

bool PluginVideoModel::canFetchMore(const QModelIndex &) const {
    return (status() != ResourcesRequest::Loading) && (!m_next.isEmpty());
}

void PluginVideoModel::fetchMore(const QModelIndex &) {
    if (!canFetchMore()) {
        return;
    }
    
    m_request->list(Resources::VIDEO, m_next);
    emit statusChanged(status());
}

QVariant PluginVideoModel::data(const QModelIndex &index, int role) const {
    if (PluginVideo *video = get(index.row())) {
        return video->property(m_roles[role]);
    }
    
    return QVariant();
}

QMap<int, QVariant> PluginVideoModel::itemData(const QModelIndex &index) const {
    QMap<int, QVariant> map;
    
    if (PluginVideo *video = get(index.row())) {
        QHashIterator<int, QByteArray> iterator(m_roles);
        
        while (iterator.hasNext()) {
            iterator.next();
            map[iterator.key()] = video->property(iterator.value());
        }
    }
    
    return map;
}

QVariant PluginVideoModel::data(int row, const QByteArray &role) const {
    if (PluginVideo *video = get(row)) {
        return video->property(role);
    }
    
    return QVariant();
}

QVariantMap PluginVideoModel::itemData(int row) const {
    QVariantMap map;
    
    if (PluginVideo *video = get(row)) {
        foreach (QByteArray role, m_roles.values()) {
            map[role] = video->property(role);
        }
    }
    
    return map;
}

PluginVideo* PluginVideoModel::get(int row) const {
    if ((row >= 0) && (row < m_items.size())) {
        return m_items.at(row);
    }
    
    return 0;
}

void PluginVideoModel::list(const QString &id) {
    if (status() == ResourcesRequest::Loading) {
        return;
    }
    
    clear();
    m_id = id;
    m_query = QString();
    m_request->list(Resources::VIDEO, id);
    emit statusChanged(status());
}

void PluginVideoModel::search(const QString &query, const QString &order) {
    if (status() == ResourcesRequest::Loading) {
        return;
    }
    
    clear();
    m_id = QString();
    m_query = query;
    m_order = order;
    m_request->search(Resources::VIDEO, query, order);
    emit statusChanged(status());
}

void PluginVideoModel::clear() {
    if (!m_items.isEmpty()) {
        beginResetModel();
        qDeleteAll(m_items);
        m_items.clear();
        m_next = QString();
        endResetModel();
        emit countChanged(rowCount());
    }
}

void PluginVideoModel::cancel() {
    m_request->cancel();
}

void PluginVideoModel::reload() {
    clear();
    
    if (m_query.isEmpty()) {
        m_request->list(Resources::VIDEO, m_id);
    }
    else {
        m_request->search(Resources::VIDEO, m_query, m_order);
    }
    
    emit statusChanged(status());
}

void PluginVideoModel::append(PluginVideo *video) {
    beginInsertRows(QModelIndex(), m_items.size(), m_items.size());
    m_items << video;
    endInsertRows();
}

void PluginVideoModel::insert(int row, PluginVideo *video) {
    if ((row >= 0) && (row < m_items.size())) {
        beginInsertRows(QModelIndex(), row, row);
        m_items.insert(row, video);
        endInsertRows();
    }
    else {
        append(video);
    }
}

void PluginVideoModel::remove(int row) {
    if ((row >= 0) && (row < m_items.size())) {
        beginRemoveRows(QModelIndex(), row, row);
        m_items.takeAt(row)->deleteLater();
        endRemoveRows();
    }
}

void PluginVideoModel::onRequestFinished() {
    if (m_request->status() == ResourcesRequest::Ready) {
        QVariantMap result = m_request->result().toMap();
        
        if (!result.isEmpty()) {
            m_next = result.value("next").toString();
            QVariantList list = result.value("items").toList();

            beginInsertRows(QModelIndex(), m_items.size(), m_items.size() + list.size() - 1);
    
            foreach (QVariant item, list) {
                m_items << new PluginVideo(service(), item.toMap(), this);
            }

            endInsertRows();
            emit countChanged(rowCount());
                
        }
    }
    
    emit statusChanged(status());
}
