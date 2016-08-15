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

#include "youtubecommentmodel.h"
#include "logger.h"
#include "youtube.h"

YouTubeCommentModel::YouTubeCommentModel(QObject *parent) :
    QAbstractListModel(parent),
    m_request(new QYouTube::ResourcesRequest(this))
{
    m_roles[BodyRole] = "body";
    m_roles[DateRole] = "date";
    m_roles[IdRole] = "id";
    m_roles[ParentIdRole] = "parentId";
    m_roles[ThumbnailUrlRole] = "thumbnailUrl";
    m_roles[UserIdRole] = "userId";
    m_roles[UsernameRole] = "username";
    m_roles[VideoIdRole] = "videoId";
#if QT_VERSION < 0x050000
    setRoleNames(m_roles);
#endif
    m_request->setApiKey(YouTube::apiKey());
    m_request->setClientId(YouTube::clientId());
    m_request->setClientSecret(YouTube::clientSecret());
    m_request->setAccessToken(YouTube::accessToken());
    m_request->setRefreshToken(YouTube::refreshToken());
    
    connect(m_request, SIGNAL(accessTokenChanged(QString)), YouTube::instance(), SLOT(setAccessToken(QString)));
    connect(m_request, SIGNAL(refreshTokenChanged(QString)), YouTube::instance(), SLOT(setRefreshToken(QString)));
    connect(m_request, SIGNAL(finished()), this, SLOT(onRequestFinished()));
    connect(YouTube::instance(), SIGNAL(commentAdded(YouTubeComment*)),
            this, SLOT(onCommentAdded(YouTubeComment*)));
}

QString YouTubeCommentModel::errorString() const {
    return YouTube::getErrorString(m_request->result().toMap());
}

QYouTube::ResourcesRequest::Status YouTubeCommentModel::status() const {
    return m_request->status();
}

#if QT_VERSION >=0x050000
QHash<int, QByteArray> YouTubeCommentModel::roleNames() const {
    return m_roles;
}
#endif

int YouTubeCommentModel::rowCount(const QModelIndex &) const {
    return m_items.size();
}

bool YouTubeCommentModel::canFetchMore(const QModelIndex &) const {
    return (status() != QYouTube::ResourcesRequest::Loading) && (!m_nextPageToken.isEmpty());
}

void YouTubeCommentModel::fetchMore(const QModelIndex &) {
    if (!canFetchMore()) {
        return;
    }
    
    QVariantMap params = m_params;
    params["pageToken"] = m_nextPageToken;
    
    m_request->list(m_resourcePath, m_part, m_filters, params);
    emit statusChanged(status());
}

QVariant YouTubeCommentModel::data(const QModelIndex &index, int role) const {
    if (const YouTubeComment *comment = get(index.row())) {
        return comment->property(m_roles[role]);
    }
    
    return QVariant();
}

QMap<int, QVariant> YouTubeCommentModel::itemData(const QModelIndex &index) const {
    QMap<int, QVariant> map;
    
    if (const YouTubeComment *comment = get(index.row())) {
        QHashIterator<int, QByteArray> iterator(m_roles);
        
        while (iterator.hasNext()) {
            iterator.next();
            map[iterator.key()] = comment->property(iterator.value());
        }
    }
    
    return map;
}

QVariant YouTubeCommentModel::data(int row, const QByteArray &role) const {
    if (const YouTubeComment *comment = get(row)) {
        return comment->property(role);
    }
    
    return QVariant();
}

QVariantMap YouTubeCommentModel::itemData(int row) const {
    QVariantMap map;
    
    if (const YouTubeComment *comment = get(row)) {
        foreach (const QByteArray &role, m_roles.values()) {
            map[role] = comment->property(role);
        }
    }
    
    return map;
}

YouTubeComment* YouTubeCommentModel::get(int row) const {
    if ((row >= 0) && (row < m_items.size())) {
        return m_items.at(row);
    }
    
    return 0;
}

void YouTubeCommentModel::list(const QString &resourcePath, const QStringList &part, const QVariantMap &filters,
                               const QVariantMap &params) {
    if (status() == QYouTube::ResourcesRequest::Loading) {
        return;
    }
    
    Logger::log("YouTubeCommentModel::list(). Resource path: " + resourcePath, Logger::HighVerbosity);
    clear();
    m_resourcePath = resourcePath;
    m_part = part;
    m_filters = filters;
    m_params = params;
    m_request->list(resourcePath, part, filters, params);
    emit statusChanged(status());
}

void YouTubeCommentModel::clear() {
    if (!m_items.isEmpty()) {
        beginResetModel();
        qDeleteAll(m_items);
        m_items.clear();
        m_nextPageToken = QString();
        endResetModel();
        emit countChanged(rowCount());
    }
}

void YouTubeCommentModel::cancel() {
    m_request->cancel();
}

void YouTubeCommentModel::reload() {
    if (status() == QYouTube::ResourcesRequest::Loading) {
        return;
    }
    
    Logger::log("YouTubeCommentModel::reload(). Resource path: " + m_resourcePath, Logger::HighVerbosity);
    clear();
    m_request->list(m_resourcePath, m_part, m_filters, m_params);
    emit statusChanged(status());
}

void YouTubeCommentModel::append(YouTubeComment *comment) {
    beginInsertRows(QModelIndex(), m_items.size(), m_items.size());
    m_items << comment;
    endInsertRows();
}

void YouTubeCommentModel::insert(int row, YouTubeComment *comment) {
    if ((row >= 0) && (row < m_items.size())) {
        beginInsertRows(QModelIndex(), row, row);
        m_items.insert(row, comment);
        endInsertRows();
    }
    else {
        append(comment);
    }
}

void YouTubeCommentModel::remove(int row) {
    if ((row >= 0) && (row < m_items.size())) {
        beginRemoveRows(QModelIndex(), row, row);
        m_items.takeAt(row)->deleteLater();
        endRemoveRows();
    }
}

void YouTubeCommentModel::onRequestFinished() {
    if (m_request->status() == QYouTube::ResourcesRequest::Ready) {
        const QVariantMap result = m_request->result().toMap();
        
        if (!result.isEmpty()) {
            m_nextPageToken = result.value("nextPageToken").toString();
            const QVariantList list = result.value("items").toList();
    
            if (result.value("kind") == "youtube#commentThreadListResponse") {
                foreach (QVariant item, list) {
                    const QVariantMap thread = item.toMap();
                    append(new YouTubeComment(thread.value("snippet").toMap().value("topLevelComment").toMap(), this));
                    
                    foreach (const QVariant &reply, thread.value("replies").toList()) {
                        append(new YouTubeComment(reply.toMap(), this));
                    }
                }
            }
            else {
                beginInsertRows(QModelIndex(), m_items.size(), m_items.size() + list.size() - 1);
                
                foreach (const QVariant &item, list) {
                    m_items << new YouTubeComment(item.toMap(), this);
                }
                
                endInsertRows();
            }
            
            emit countChanged(rowCount());      
        }
    }
    else {
        Logger::log("YouTubeCommentModel::onRequestFinished(). Error: " + errorString());
    }
    
    emit statusChanged(status());
}

void YouTubeCommentModel::onCommentAdded(YouTubeComment *comment) {
    if (m_filters.value("videoId") == comment->videoId()) {
        if (comment->parentId().isEmpty()) {
            insert(0, new YouTubeComment(comment, this));
        }
        else {
            const QModelIndexList list = match(index(0), IdRole, comment->parentId(), 1, Qt::MatchExactly);
            insert(list.isEmpty() ? 0 : list.first().row() + 1, new YouTubeComment(comment, this));
        }
    }
}
