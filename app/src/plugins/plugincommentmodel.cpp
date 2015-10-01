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

#include "plugincommentmodel.h"
#include "resources.h"

PluginCommentModel::PluginCommentModel(QObject *parent) :
    QAbstractListModel(parent),
    m_request(new ResourcesRequest(this))
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
    connect(m_request, SIGNAL(serviceChanged()), this, SIGNAL(serviceChanged()));
    connect(m_request, SIGNAL(finished()), this, SLOT(onRequestFinished()));
}

QString PluginCommentModel::service() const {
    return m_request->service();
}

void PluginCommentModel::setService(const QString &s) {
    m_request->setService(s);
}

QString PluginCommentModel::errorString() const {
    return m_request->errorString();
}

ResourcesRequest::Status PluginCommentModel::status() const {
    return m_request->status();
}

#if QT_VERSION >=0x050000
QHash<int, QByteArray> PluginCommentModel::roleNames() const {
    return m_roles;
}
#endif

int PluginCommentModel::rowCount(const QModelIndex &) const {
    return m_items.size();
}

bool PluginCommentModel::canFetchMore(const QModelIndex &) const {
    return (status() != ResourcesRequest::Loading) && (!m_next.isEmpty());
}

void PluginCommentModel::fetchMore(const QModelIndex &) {
    if (!canFetchMore()) {
        return;
    }
    
    m_request->list(Resources::COMMENT, m_next);
    emit statusChanged(status());
}

QVariant PluginCommentModel::data(const QModelIndex &index, int role) const {
    if (PluginComment *comment = get(index.row())) {
        return comment->property(m_roles[role]);
    }
    
    return QVariant();
}

QMap<int, QVariant> PluginCommentModel::itemData(const QModelIndex &index) const {
    QMap<int, QVariant> map;
    
    if (PluginComment *comment = get(index.row())) {
        QHashIterator<int, QByteArray> iterator(m_roles);
        
        while (iterator.hasNext()) {
            iterator.next();
            map[iterator.key()] = comment->property(iterator.value());
        }
    }
    
    return map;
}

QVariant PluginCommentModel::data(int row, const QByteArray &role) const {
    if (PluginComment *comment = get(row)) {
        return comment->property(role);
    }
    
    return QVariant();
}

QVariantMap PluginCommentModel::itemData(int row) const {
    QVariantMap map;
    
    if (PluginComment *comment = get(row)) {
        foreach (QByteArray role, m_roles.values()) {
            map[role] = comment->property(role);
        }
    }
    
    return map;
}

PluginComment* PluginCommentModel::get(int row) const {
    if ((row >= 0) && (row < m_items.size())) {
        return m_items.at(row);
    }
    
    return 0;
}

void PluginCommentModel::list(const QString &id) {
    if (status() == ResourcesRequest::Loading) {
        return;
    }
    
    clear();
    m_id = id;
    m_query = QString();
    m_request->list(Resources::COMMENT, id);
    emit statusChanged(status());
}

void PluginCommentModel::search(const QString &query, const QString &order) {
    if (status() == ResourcesRequest::Loading) {
        return;
    }
    
    clear();
    m_id = QString();
    m_query = query;
    m_order = order;
    m_request->search(Resources::COMMENT, query, order);
    emit statusChanged(status());
}

void PluginCommentModel::clear() {
    if (!m_items.isEmpty()) {
        beginResetModel();
        qDeleteAll(m_items);
        m_items.clear();
        m_next = QString();
        endResetModel();
        emit countChanged(rowCount());
    }
}

void PluginCommentModel::cancel() {
    m_request->cancel();
}

void PluginCommentModel::reload() {
    clear();
    
    if (m_query.isEmpty()) {
        m_request->list(Resources::COMMENT, m_id);
    }
    else {
        m_request->search(Resources::COMMENT, m_query, m_order);
    }
    
    emit statusChanged(status());
}

void PluginCommentModel::append(PluginComment *comment) {
    beginInsertRows(QModelIndex(), m_items.size(), m_items.size());
    m_items << comment;
    endInsertRows();
}

void PluginCommentModel::insert(int row, PluginComment *comment) {
    if ((row >= 0) && (row < m_items.size())) {
        beginInsertRows(QModelIndex(), row, row);
        m_items.insert(row, comment);
        endInsertRows();
    }
    else {
        append(comment);
    }
}

void PluginCommentModel::remove(int row) {
    if ((row >= 0) && (row < m_items.size())) {
        beginRemoveRows(QModelIndex(), row, row);
        m_items.takeAt(row)->deleteLater();
        endRemoveRows();
    }
}

void PluginCommentModel::onRequestFinished() {
    if (m_request->status() == ResourcesRequest::Ready) {
        QVariantMap result = m_request->result().toMap();
        
        if (!result.isEmpty()) {
            m_next = result.value("next").toString();
            QVariantList list = result.value("items").toList();

            beginInsertRows(QModelIndex(), m_items.size(), m_items.size() + list.size() - 1);
    
            foreach (QVariant item, list) {
                m_items << new PluginComment(service(), item.toMap(), this);
            }

            endInsertRows();
            emit countChanged(rowCount());
                
        }
    }
    
    emit statusChanged(status());
}
