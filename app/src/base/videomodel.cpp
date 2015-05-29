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

#include "videomodel.h"
#ifdef CUTETUBE_DEBUG
#include <QDebug>
#endif

VideoModel::VideoModel(QObject *parent) :
    QAbstractListModel(parent)
{
    m_roles[DateRole] = "date";
    m_roles[DescriptionRole] = "description";
    m_roles[DurationRole] = "duration";
    m_roles[IdRole] = "id";
    m_roles[LargeThumbnailUrlRole] = "largeThumbnailUrl";
    m_roles[ServiceRole] = "service";
    m_roles[StreamUrlRole] = "streamUrl";
    m_roles[ThumbnailUrlRole] = "thumbnailUrl";
    m_roles[TitleRole] = "title";
    m_roles[UrlRole] = "url";
    m_roles[ViewCountRole] = "viewCount";
#if QT_VERSION < 0x050000
    setRoleNames(m_roles);
#endif
}

#if QT_VERSION >=0x050000
QHash<int, QByteArray> VideoModel::roleNames() const {
    return m_roles;
}
#endif

int VideoModel::rowCount(const QModelIndex &) const {
    return m_items.size();
}

QVariant VideoModel::data(const QModelIndex &index, int role) const {
    if (CTVideo *video = get(index.row())) {
        return video->property(m_roles[role]);
    }
    
    return QVariant();
}

QMap<int, QVariant> VideoModel::itemData(const QModelIndex &index) const {
    QMap<int, QVariant> map;
    
    if (CTVideo *video = get(index.row())) {
        QHashIterator<int, QByteArray> iterator(m_roles);
        
        while (iterator.hasNext()) {
            iterator.next();
            map[iterator.key()] = video->property(iterator.value());
        }
    }
    
    return map;
}

QVariant VideoModel::data(int row, const QByteArray &role) const {
    if (CTVideo *video = get(row)) {
        return video->property(role);
    }
    
    return QVariant();
}

QVariantMap VideoModel::itemData(int row) const {
    QVariantMap map;
    
    if (CTVideo *video = get(row)) {
        foreach (QByteArray role, m_roles.values()) {
            map[role] = video->property(role);
        }
    }
    
    return map;
}

CTVideo* VideoModel::get(int row) const {
    if ((row >= 0) && (row < m_items.size())) {
        return m_items.at(row);
    }
    
    return 0;
}

void VideoModel::clear() {
    if (!m_items.isEmpty()) {
        beginResetModel();
        m_items.clear();
        endResetModel();
        emit countChanged(rowCount());
    }
}

void VideoModel::append(CTVideo *video) {
    beginInsertRows(QModelIndex(), m_items.size(), m_items.size());
    m_items << new CTVideo(video, this);
    endInsertRows();
}

void VideoModel::insert(int row, CTVideo *video) {
    if ((row >= 0) && (row < m_items.size())) {
        beginInsertRows(QModelIndex(), row, row);
        m_items.insert(row, new CTVideo(video, this));
        endInsertRows();
    }
    else {
        append(video);
    }
}

void VideoModel::remove(int row) {
    if ((row >= 0) && (row < m_items.size())) {
        beginRemoveRows(QModelIndex(), row, row);
        m_items.takeAt(row)->deleteLater();
        endRemoveRows();
    }
}
