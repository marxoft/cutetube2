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

#include "pluginplaylistmodel.h"
#include "resources.h"
#ifdef CUTETUBE_DEBUG
#include <QDebug>
#endif

PluginPlaylistModel::PluginPlaylistModel(QObject *parent) :
    QAbstractListModel(parent),
    m_request(new ResourcesRequest(this))
{
    m_roles[DateRole] = "date";
    m_roles[DescriptionRole] = "description";
    m_roles[IdRole] = "id";
    m_roles[LargeThumbnailUrlRole] = "largeThumbnailUrl";
    m_roles[ServiceRole] = "service";
    m_roles[ThumbnailUrlRole] = "thumbnailUrl";
    m_roles[TitleRole] = "title";
    m_roles[UserIdRole] = "userId";
    m_roles[UsernameRole] = "username";
    m_roles[VideoCountRole] = "videoCount";
#if QT_VERSION < 0x050000
    setRoleNames(m_roles);
#endif
    connect(m_request, SIGNAL(serviceChanged()), this, SIGNAL(serviceChanged()));
    connect(m_request, SIGNAL(finished()), this, SLOT(onRequestFinished()));
}

QString PluginPlaylistModel::service() const {
    return m_request->service();
}

void PluginPlaylistModel::setService(const QString &s) {
    m_request->setService(s);
}

QString PluginPlaylistModel::errorString() const {
    return m_request->errorString();
}

ResourcesRequest::Status PluginPlaylistModel::status() const {
    return m_request->status();
}

#if QT_VERSION >=0x050000
QHash<int, QByteArray> PluginPlaylistModel::roleNames() const {
    return m_roles;
}
#endif

int PluginPlaylistModel::rowCount(const QModelIndex &) const {
    return m_items.size();
}

bool PluginPlaylistModel::canFetchMore(const QModelIndex &) const {
    return (status() != ResourcesRequest::Loading) && (!m_next.isEmpty());
}

void PluginPlaylistModel::fetchMore(const QModelIndex &) {
    if (!canFetchMore()) {
        return;
    }
    
    m_request->list(Resources::PLAYLIST, m_next);
    emit statusChanged(status());
}

QVariant PluginPlaylistModel::data(const QModelIndex &index, int role) const {
    if (PluginPlaylist *playlist = get(index.row())) {
        return playlist->property(m_roles[role]);
    }
    
    return QVariant();
}

QMap<int, QVariant> PluginPlaylistModel::itemData(const QModelIndex &index) const {
    QMap<int, QVariant> map;
    
    if (PluginPlaylist *playlist = get(index.row())) {
        QHashIterator<int, QByteArray> iterator(m_roles);
        
        while (iterator.hasNext()) {
            iterator.next();
            map[iterator.key()] = playlist->property(iterator.value());
        }
    }
    
    return map;
}

QVariant PluginPlaylistModel::data(int row, const QByteArray &role) const {
    if (PluginPlaylist *playlist = get(row)) {
        return playlist->property(role);
    }
    
    return QVariant();
}

QVariantMap PluginPlaylistModel::itemData(int row) const {
    QVariantMap map;
    
    if (PluginPlaylist *playlist = get(row)) {
        foreach (QByteArray role, m_roles.values()) {
            map[role] = playlist->property(role);
        }
    }
    
    return map;
}

PluginPlaylist* PluginPlaylistModel::get(int row) const {
    if ((row >= 0) && (row < m_items.size())) {
        return m_items.at(row);
    }
    
    return 0;
}

void PluginPlaylistModel::list(const QString &id) {
    if (status() == ResourcesRequest::Loading) {
        return;
    }
    
    clear();
    m_id = id;
    m_query = QString();
    m_request->list(Resources::PLAYLIST, id);
    emit statusChanged(status());
}

void PluginPlaylistModel::search(const QString &query, const QString &order) {
    if (status() == ResourcesRequest::Loading) {
        return;
    }
    
    clear();
    m_id = QString();
    m_query = query;
    m_order = order;
    m_request->search(Resources::PLAYLIST, query, order);
    emit statusChanged(status());
}

void PluginPlaylistModel::clear() {
    if (!m_items.isEmpty()) {
        beginResetModel();
        qDeleteAll(m_items);
        m_items.clear();
        m_next = QString();
        endResetModel();
        emit countChanged(rowCount());
    }
}

void PluginPlaylistModel::cancel() {
    m_request->cancel();
}

void PluginPlaylistModel::reload() {
    clear();
    
    if (m_query.isEmpty()) {
        m_request->list(Resources::PLAYLIST, m_id);
    }
    else {
        m_request->search(Resources::PLAYLIST, m_query, m_order);
    }
    
    emit statusChanged(status());
}

void PluginPlaylistModel::append(PluginPlaylist *playlist) {
    beginInsertRows(QModelIndex(), m_items.size(), m_items.size());
    m_items << playlist;
    endInsertRows();
}

void PluginPlaylistModel::insert(int row, PluginPlaylist *playlist) {
    if ((row >= 0) && (row < m_items.size())) {
        beginInsertRows(QModelIndex(), row, row);
        m_items.insert(row, playlist);
        endInsertRows();
    }
    else {
        append(playlist);
    }
}

void PluginPlaylistModel::remove(int row) {
    if ((row >= 0) && (row < m_items.size())) {
        beginRemoveRows(QModelIndex(), row, row);
        m_items.takeAt(row)->deleteLater();
        endRemoveRows();
    }
}

void PluginPlaylistModel::onRequestFinished() {
    if (m_request->status() == ResourcesRequest::Ready) {
        QVariantMap result = m_request->result().toMap();
        
        if (!result.isEmpty()) {
            m_next = result.value("next").toString();
            QVariantList list = result.value("items").toList();

            beginInsertRows(QModelIndex(), m_items.size(), m_items.size() + list.size() - 1);
    
            foreach (QVariant item, list) {
                m_items << new PluginPlaylist(service(), item.toMap(), this);
            }

            endInsertRows();
            emit countChanged(rowCount());
        }
    }
    
    emit statusChanged(status());
}
