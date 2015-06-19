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

#include "dailymotioncommentmodel.h"
#include "dailymotion.h"
#ifdef CUTETUBE_DEBUG
#include <QDebug>
#endif

DailymotionCommentModel::DailymotionCommentModel(QObject *parent) :
    QAbstractListModel(parent),
    m_request(new QDailymotion::ResourcesRequest(this)),
    m_hasMore(false)
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
    m_request->setClientId(Dailymotion::instance()->clientId());
    m_request->setClientSecret(Dailymotion::instance()->clientSecret());
    m_request->setAccessToken(Dailymotion::instance()->accessToken());
    m_request->setRefreshToken(Dailymotion::instance()->refreshToken());
    
    connect(m_request, SIGNAL(accessTokenChanged(QString)), Dailymotion::instance(), SLOT(setAccessToken(QString)));
    connect(m_request, SIGNAL(refreshTokenChanged(QString)), Dailymotion::instance(), SLOT(setRefreshToken(QString)));
    connect(m_request, SIGNAL(finished()), this, SLOT(onRequestFinished()));
    connect(Dailymotion::instance(), SIGNAL(commentAdded(DailymotionComment*)),
            this, SLOT(onCommentAdded(DailymotionComment*)));
}

QString DailymotionCommentModel::errorString() const {
    return Dailymotion::getErrorString(m_request->result().toMap());
}

QDailymotion::ResourcesRequest::Status DailymotionCommentModel::status() const {
    return m_request->status();
}

#if QT_VERSION >=0x050000
QHash<int, QByteArray> DailymotionCommentModel::roleNames() const {
    return m_roles;
}
#endif

int DailymotionCommentModel::rowCount(const QModelIndex &) const {
    return m_items.size();
}

bool DailymotionCommentModel::canFetchMore(const QModelIndex &) const {
    return (status() != QDailymotion::ResourcesRequest::Loading) && (m_hasMore);
}

void DailymotionCommentModel::fetchMore(const QModelIndex &) {
    if (!canFetchMore()) {
        return;
    }
    
    const int page = m_filters.value("page").toInt();
    m_filters["page"] = (page > 0 ? page + 1 : 2);
    m_request->list(m_resourcePath, m_filters, Dailymotion::COMMENT_FIELDS);
    emit statusChanged(status());
}

QVariant DailymotionCommentModel::data(const QModelIndex &index, int role) const {
    if (DailymotionComment *user = get(index.row())) {
        return user->property(m_roles[role]);
    }
    
    return QVariant();
}

QMap<int, QVariant> DailymotionCommentModel::itemData(const QModelIndex &index) const {
    QMap<int, QVariant> map;
    
    if (DailymotionComment *user = get(index.row())) {
        QHashIterator<int, QByteArray> iterator(m_roles);
        
        while (iterator.hasNext()) {
            iterator.next();
            map[iterator.key()] = user->property(iterator.value());
        }
    }
    
    return map;
}

QVariant DailymotionCommentModel::data(int row, const QByteArray &role) const {
    if (DailymotionComment *user = get(row)) {
        return user->property(role);
    }
    
    return QVariant();
}

QVariantMap DailymotionCommentModel::itemData(int row) const {
    QVariantMap map;
    
    if (DailymotionComment *user = get(row)) {
        foreach (QByteArray role, m_roles.values()) {
            map[role] = user->property(role);
        }
    }
    
    return map;
}

DailymotionComment* DailymotionCommentModel::get(int row) const {
    if ((row >= 0) && (row < m_items.size())) {
        return m_items.at(row);
    }
    
    return 0;
}

void DailymotionCommentModel::list(const QString &resourcePath, const QVariantMap &filters) {
    if (status() == QDailymotion::ResourcesRequest::Loading) {
        return;
    }
    
    clear();
    m_resourcePath = resourcePath;
    m_filters = filters;
    m_request->list(resourcePath, filters, Dailymotion::COMMENT_FIELDS);
    emit statusChanged(status());    
}

void DailymotionCommentModel::clear() {
    if (!m_items.isEmpty()) {
        beginResetModel();
        qDeleteAll(m_items);
        m_items.clear();
        m_hasMore = false;
        endResetModel();
        emit countChanged(rowCount());
    }
}

void DailymotionCommentModel::cancel() {
    m_request->cancel();
}

void DailymotionCommentModel::reload() {
    clear();
    m_request->list(m_resourcePath,  m_filters, Dailymotion::COMMENT_FIELDS);
    emit statusChanged(status());
}

void DailymotionCommentModel::append(DailymotionComment *user) {
    beginInsertRows(QModelIndex(), m_items.size(), m_items.size());
    m_items << user;
    endInsertRows();
}

void DailymotionCommentModel::insert(int row, DailymotionComment *user) {
    if ((row >= 0) && (row < m_items.size())) {
        beginInsertRows(QModelIndex(), row, row);
        m_items.insert(row, user);
        endInsertRows();
    }
    else {
        append(user);
    }
}

void DailymotionCommentModel::remove(int row) {
    if ((row >= 0) && (row < m_items.size())) {
        beginRemoveRows(QModelIndex(), row, row);
        m_items.takeAt(row)->deleteLater();
        endRemoveRows();
    }
}

void DailymotionCommentModel::onRequestFinished() {
    if (m_request->status() == QDailymotion::ResourcesRequest::Ready) {
        QVariantMap result = m_request->result().toMap();
        
        if (!result.isEmpty()) {
            m_hasMore = result.value("has_more").toBool();
            QVariantList list = result.value("list").toList();

            beginInsertRows(QModelIndex(), m_items.size(), m_items.size() + list.size() - 1);
    
            foreach (QVariant item, list) {
                m_items << new DailymotionComment(item.toMap(), this);
            }

            endInsertRows();
            emit countChanged(rowCount());
        }
    }
    
    emit statusChanged(status());
}

void DailymotionCommentModel::onCommentAdded(DailymotionComment *comment) {
    if (comment->videoId() == m_resourcePath.section('/', 1, 1)) {
        insert(0, new DailymotionComment(comment, this));
    }
#ifdef CUTETUBE_DEBUG
    qDebug() << "DailymotionCommentModel::onCommentAdded" << comment->videoId();
#endif
}
