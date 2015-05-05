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

#include "vimeocommentmodel.h"
#include "vimeo.h"
#ifdef CUTETUBE_DEBUG
#include <QDebug>
#endif

VimeoCommentModel::VimeoCommentModel(QObject *parent) :
    QAbstractListModel(parent),
    m_request(new QVimeo::ResourcesRequest(this))
{
    m_roles[BodyRole] = "body";
    m_roles[DateRole] = "date";
    m_roles[IdRole] = "id";
    m_roles[ThumbnailUrlRole] = "thumbnailUrl";
    m_roles[UserIdRole] = "userId";
    m_roles[UsernameRole] = "username";
    m_roles[VideoIdRole] = "videoId";
#if QT_VERSION < 0x050000
    setRoleNames(m_roles);
#endif
    m_request->setClientId(Vimeo::instance()->clientId());
    m_request->setClientSecret(Vimeo::instance()->clientSecret());
    m_request->setAccessToken(Vimeo::instance()->accessToken());
    
    connect(m_request, SIGNAL(accessTokenChanged(QString)), Vimeo::instance(), SLOT(setAccessToken(QString)));
    connect(m_request, SIGNAL(finished()), this, SLOT(onRequestFinished()));
    connect(Vimeo::instance(), SIGNAL(commentAdded(const VimeoComment*)),
            this, SLOT(onCommentAdded(const VimeoComment*)));
}

QString VimeoCommentModel::errorString() const {
    return Vimeo::getErrorString(m_request->result().toMap());
}

QVimeo::ResourcesRequest::Status VimeoCommentModel::status() const {
    return m_request->status();
}

#if QT_VERSION >=0x050000
QHash<int, QByteArray> VimeoCommentModel::roleNames() const {
    return m_roles;
}
#endif

int VimeoCommentModel::rowCount(const QModelIndex &) const {
    return m_items.size();
}

bool VimeoCommentModel::canFetchMore(const QModelIndex &) const {
    return (status() != QVimeo::ResourcesRequest::Loading) && (m_hasMore);
}

void VimeoCommentModel::fetchMore(const QModelIndex &) {
    if (!canFetchMore()) {
        return;
    }
    
    const int page = m_filters.value("page").toInt();
    m_filters["page"] = (page > 0 ? page + 1 : 2);
    m_request->list(m_resourcePath, m_filters);
    emit statusChanged(status());
}

QVariant VimeoCommentModel::data(const QModelIndex &index, int role) const {
    if (VimeoComment *comment = get(index.row())) {
        return comment->property(m_roles[role]);
    }
    
    return QVariant();
}

QMap<int, QVariant> VimeoCommentModel::itemData(const QModelIndex &index) const {
    QMap<int, QVariant> map;
    
    if (VimeoComment *comment = get(index.row())) {
        QHashIterator<int, QByteArray> iterator(m_roles);
        
        while (iterator.hasNext()) {
            iterator.next();
            map[iterator.key()] = comment->property(iterator.value());
        }
    }
    
    return map;
}

QVariant VimeoCommentModel::data(int row, const QByteArray &role) const {
    if (VimeoComment *comment = get(row)) {
        return comment->property(role);
    }
    
    return QVariant();
}

QVariantMap VimeoCommentModel::itemData(int row) const {
    QVariantMap map;
    
    if (VimeoComment *comment = get(row)) {
        foreach (QByteArray role, m_roles.values()) {
            map[role] = comment->property(role);
        }
    }
    
    return map;
}

VimeoComment* VimeoCommentModel::get(int row) const {
    if ((row >= 0) && (row < m_items.size())) {
        return m_items.at(row);
    }
    
    return 0;
}

void VimeoCommentModel::list(const QString &resourcePath, const QVariantMap &filters) {
    if (status() == QVimeo::ResourcesRequest::Loading) {
        return;
    }
    
    clear();
    m_resourcePath = resourcePath;
    m_filters = filters;
    m_request->list(resourcePath, filters);
    emit statusChanged(status());
}

void VimeoCommentModel::clear() {
    if (!m_items.isEmpty()) {
        beginResetModel();
        m_items.clear();
        endResetModel();
        emit countChanged(rowCount());
    }
}

void VimeoCommentModel::cancel() {
    m_request->cancel();
}

void VimeoCommentModel::reload() {
    clear();
    m_request->list(m_resourcePath, m_filters);
    emit statusChanged(status());
}

void VimeoCommentModel::append(VimeoComment *comment) {
    beginInsertRows(QModelIndex(), m_items.size(), m_items.size());
    m_items << comment;
    endInsertRows();
}

void VimeoCommentModel::insert(int row, VimeoComment *comment) {
    if ((row >= 0) && (row < m_items.size())) {
        beginInsertRows(QModelIndex(), row, row);
        m_items.insert(row, comment);
        endInsertRows();
    }
    else {
        append(comment);
    }
}

void VimeoCommentModel::remove(int row) {
    if ((row >= 0) && (row < m_items.size())) {
        beginRemoveRows(QModelIndex(), row, row);
        m_items.takeAt(row)->deleteLater();
        endRemoveRows();
    }
}

void VimeoCommentModel::onRequestFinished() {
    if (m_request->status() == QVimeo::ResourcesRequest::Ready) {
        QVariantMap result = m_request->result().toMap();
        
        if (!result.isEmpty()) {
            m_hasMore = !result.value("paging").toMap().value("next").isNull();
            QVariantList list = result.value("data").toList();

            beginInsertRows(QModelIndex(), m_items.size(), m_items.size() + list.size() - 1);
    
            foreach (QVariant item, list) {
                m_items << new VimeoComment(item.toMap(), this);
            }

            endInsertRows();
            emit countChanged(rowCount());
        }
    }
    
    emit statusChanged(status());
}

void VimeoCommentModel::onCommentAdded(const VimeoComment *comment) {
    if (comment->videoId() == m_resourcePath.section('/', 1, 1)) {
        insert(0, new VimeoComment(comment, this));
    }
#ifdef CUTETUBE_DEBUG
    qDebug() << "VimeoPlaylistModel::onCommentAdded" << comment->videoId();
#endif
}
