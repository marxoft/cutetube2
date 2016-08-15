/*
 * Copyright (C) 2016 Stuart Howarth <showarth@marxoft.co.uk>
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms and conditions of the GNU General Public License,
 * version 3, as published by the Free Software Foundation.
 *
 * This program is distributed in the hope it will be useful, but WITHOUT ANY
 * WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software Foundation,
 * Inc., 51 Franklin St - Fifth Floor, Boston, MA 02110-1301 USA.
 */

#include "plugincommentmodel.h"
#include "logger.h"
#include "pluginmanager.h"
#include "resources.h"

PluginCommentModel::PluginCommentModel(QObject *parent) :
    QAbstractListModel(parent),
    m_request(0)
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
}

QString PluginCommentModel::errorString() const {
    return m_request ? m_request->errorString() : QString();
}

QString PluginCommentModel::service() const {
    return m_service;
}

void PluginCommentModel::setService(const QString &s) {
    if (s != service()) {
        m_service = s;
        emit serviceChanged();

        clear();

        if (m_request) {
            m_request->deleteLater();
            m_request = 0;
        }
    }
}

ResourcesRequest::Status PluginCommentModel::status() const {
    return m_request ? m_request->status() : ResourcesRequest::Null;
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

    if (ResourcesRequest *r = request()) {
        r->list(Resources::COMMENT, m_next);
        emit statusChanged(status());
    }
}

QVariant PluginCommentModel::data(const QModelIndex &index, int role) const {
    if (const PluginComment *comment = get(index.row())) {
        return comment->property(m_roles[role]);
    }
    
    return QVariant();
}

QMap<int, QVariant> PluginCommentModel::itemData(const QModelIndex &index) const {
    QMap<int, QVariant> map;
    
    if (const PluginComment *comment = get(index.row())) {
        QHashIterator<int, QByteArray> iterator(m_roles);
        
        while (iterator.hasNext()) {
            iterator.next();
            map[iterator.key()] = comment->property(iterator.value());
        }
    }
    
    return map;
}

QVariant PluginCommentModel::data(int row, const QByteArray &role) const {
    if (const PluginComment *comment = get(row)) {
        return comment->property(role);
    }
    
    return QVariant();
}

QVariantMap PluginCommentModel::itemData(int row) const {
    QVariantMap map;
    
    if (const PluginComment *comment = get(row)) {
        foreach (const QByteArray &role, m_roles.values()) {
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

void PluginCommentModel::list(const QString &resourceId) {
    if (status() == ResourcesRequest::Loading) {
        return;
    }
    
    Logger::log("PluginCommentModel::list(). Resource ID: " + resourceId, Logger::MediumVerbosity);
    clear();
    m_resourceId = resourceId;
    m_query = QString();

    if (ResourcesRequest *r = request()) {
        r->list(Resources::COMMENT, resourceId);
        emit statusChanged(status());
    }
}

void PluginCommentModel::search(const QString &query, const QString &order) {
    if (status() == ResourcesRequest::Loading) {
        return;
    }
    
    Logger::log(QString("PluginCommentModel::search(). Query: %1, Order: %2").arg(query).arg(order),
                Logger::MediumVerbosity);
    clear();
    m_resourceId = QString();
    m_query = query;
    m_order = order;

    if (ResourcesRequest *r = request()) {
        r->search(Resources::COMMENT, query, order);
        emit statusChanged(status());
    }
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
    if (m_request) {
        m_request->cancel();
    }
}

void PluginCommentModel::reload() {
    if (status() == ResourcesRequest::Loading) {
        return;
    }
    
    Logger::log("PluginCommentModel::reload(). Resource ID: " + m_resourceId, Logger::MediumVerbosity);
    clear();

    if (ResourcesRequest *r = request()) {
        if (m_query.isEmpty()) {
            r->list(Resources::COMMENT, m_resourceId);
        }
        else {
            r->search(Resources::COMMENT, m_query, m_order);
        }
        
        emit statusChanged(status());
    }
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

ResourcesRequest* PluginCommentModel::request() {
    if (!m_request) {
        m_request = PluginManager::instance()->createRequestForService(service(), this);

        if (m_request) {
            connect(m_request, SIGNAL(finished()), this, SLOT(onRequestFinished()));
        }
    }

    return m_request;
}

void PluginCommentModel::onRequestFinished() {
    if (m_request->status() == ResourcesRequest::Ready) {
        const QVariantMap result = m_request->result().toMap();
        
        if (!result.isEmpty()) {
            m_next = result.value("next").toString();
            const QVariantList list = result.value("items").toList();

            beginInsertRows(QModelIndex(), m_items.size(), m_items.size() + list.size() - 1);
    
            foreach (const QVariant &item, list) {
                m_items << new PluginComment(service(), item.toMap(), this);
            }

            endInsertRows();
            emit countChanged(rowCount());
                
        }
    }
    else {
        Logger::log("PluginCommentModel::onRequestFinished(). Error: " + errorString());
    }
    
    emit statusChanged(status());
}
