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

#include "youtubeplaylistmodel.h"
#include "logger.h"
#include "youtube.h"

YouTubePlaylistModel::YouTubePlaylistModel(QObject *parent) :
    QAbstractListModel(parent),
    m_request(new QYouTube::ResourcesRequest(this)),
    m_contentRequest(0)
{
    m_roles[DateRole] = "date";
    m_roles[DescriptionRole] = "description";
    m_roles[IdRole] = "id";
    m_roles[LargeThumbnailUrlRole] = "largeThumbnailUrl";
    m_roles[ThumbnailUrlRole] = "thumbnailUrl";
    m_roles[TitleRole] = "title";
    m_roles[UserIdRole] = "userId";
    m_roles[UsernameRole] = "username";
    m_roles[VideoCountRole] = "videoCount";
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
}

QString YouTubePlaylistModel::errorString() const {
    return YouTube::getErrorString(m_request->result().toMap());
}

QYouTube::ResourcesRequest::Status YouTubePlaylistModel::status() const {
    if ((m_contentRequest) && (m_contentRequest->status() == QYouTube::ResourcesRequest::Loading)) {
        return QYouTube::ResourcesRequest::Loading;
    }
    
    return m_request->status();
}

#if QT_VERSION >=0x050000
QHash<int, QByteArray> YouTubePlaylistModel::roleNames() const {
    return m_roles;
}
#endif

int YouTubePlaylistModel::rowCount(const QModelIndex &) const {
    return m_items.size();
}

bool YouTubePlaylistModel::canFetchMore(const QModelIndex &) const {
    return (status() != QYouTube::ResourcesRequest::Loading) && (!m_nextPageToken.isEmpty());
}

void YouTubePlaylistModel::fetchMore(const QModelIndex &) {
    if (!canFetchMore()) {
        return;
    }
    
    QVariantMap params = m_params;
    params["pageToken"] = m_nextPageToken;
    
    m_request->list(m_resourcePath, m_part, m_filters, params);
    emit statusChanged(status());
}

QVariant YouTubePlaylistModel::data(const QModelIndex &index, int role) const {
    if (const YouTubePlaylist *playlist = get(index.row())) {
        return playlist->property(m_roles[role]);
    }
    
    return QVariant();
}

QMap<int, QVariant> YouTubePlaylistModel::itemData(const QModelIndex &index) const {
    QMap<int, QVariant> map;
    
    if (const YouTubePlaylist *playlist = get(index.row())) {
        QHashIterator<int, QByteArray> iterator(m_roles);
        
        while (iterator.hasNext()) {
            iterator.next();
            map[iterator.key()] = playlist->property(iterator.value());
        }
    }
    
    return map;
}

QVariant YouTubePlaylistModel::data(int row, const QByteArray &role) const {
    if (const YouTubePlaylist *playlist = get(row)) {
        return playlist->property(role);
    }
    
    return QVariant();
}

QVariantMap YouTubePlaylistModel::itemData(int row) const {
    QVariantMap map;
    
    if (const YouTubePlaylist *playlist = get(row)) {
        foreach (const QByteArray &role, m_roles.values()) {
            map[role] = playlist->property(role);
        }
    }
    
    return map;
}

YouTubePlaylist* YouTubePlaylistModel::get(int row) const {
    if ((row >= 0) && (row < m_items.size())) {
        return m_items.at(row);
    }
    
    return 0;
}

void YouTubePlaylistModel::list(const QString &resourcePath, const QStringList &part, const QVariantMap &filters,
                                const QVariantMap &params) {
    if (status() == QYouTube::ResourcesRequest::Loading) {
        return;
    }
    
    Logger::log("YouTubePlaylistModel::list(). Resource path: " + resourcePath, Logger::HighVerbosity);
    clear();
    m_resourcePath = resourcePath;
    m_part = part;
    m_filters = filters;
    m_params = params;
    m_request->list(resourcePath, part, filters, params);
    emit statusChanged(status());
    
    disconnect(YouTube::instance(), 0, this, 0);
    
    if ((filters.contains("mine")) || (filters.value("channelId") == YouTube::userId())) {
        connect(YouTube::instance(), SIGNAL(playlistCreated(YouTubePlaylist*)),
                this, SLOT(onPlaylistCreated(YouTubePlaylist*)));
        connect(YouTube::instance(), SIGNAL(playlistDeleted(YouTubePlaylist*)),
                this, SLOT(onPlaylistDeleted(YouTubePlaylist*)));
    }
}

void YouTubePlaylistModel::clear() {
    if (!m_items.isEmpty()) {
        beginResetModel();
        qDeleteAll(m_items);
        m_items.clear();
        m_nextPageToken = QString();
        endResetModel();
        emit countChanged(rowCount());
    }
}

void YouTubePlaylistModel::cancel() {
    m_request->cancel();
}

void YouTubePlaylistModel::reload() {
    if (status() == QYouTube::ResourcesRequest::Loading) {
        return;
    }
    
    Logger::log("YouTubePlaylistModel::reload(). Resource path: " + m_resourcePath, Logger::HighVerbosity);
    clear();
    m_request->list(m_resourcePath, m_part, m_filters, m_params);
    emit statusChanged(status());
}

void YouTubePlaylistModel::getAdditionalContent() {
    if (!m_contentRequest) {
        m_contentRequest = new QYouTube::ResourcesRequest(this);
        m_contentRequest->setApiKey(YouTube::apiKey());
        m_contentRequest->setClientId(YouTube::clientId());
        m_contentRequest->setClientSecret(YouTube::clientSecret());
        m_contentRequest->setAccessToken(YouTube::accessToken());
        m_contentRequest->setRefreshToken(YouTube::refreshToken());

        connect(m_contentRequest, SIGNAL(accessTokenChanged(QString)),
                YouTube::instance(), SLOT(setAccessToken(QString)));
        connect(m_contentRequest, SIGNAL(refreshTokenChanged(QString)),
                YouTube::instance(), SLOT(setRefreshToken(QString)));
        connect(m_contentRequest, SIGNAL(finished()), this, SLOT(onContentRequestFinished()));
    }
    
    QStringList ids;
    
    for (int i = 0; i < m_results.size(); i++) {
        ids << m_results.at(i).first;
    }
    
    QVariantMap filters;
    filters["id"] = ids.join(",");
    
    m_contentRequest->list("/playlists", QStringList() << "contentDetails", filters);
}

void YouTubePlaylistModel::append(YouTubePlaylist *playlist) {
    beginInsertRows(QModelIndex(), m_items.size(), m_items.size());
    m_items << playlist;
    endInsertRows();
}

void YouTubePlaylistModel::insert(int row, YouTubePlaylist *playlist) {
    if ((row >= 0) && (row < m_items.size())) {
        beginInsertRows(QModelIndex(), row, row);
        m_items.insert(row, playlist);
        endInsertRows();
    }
    else {
        append(playlist);
    }
}

void YouTubePlaylistModel::remove(int row) {
    if ((row >= 0) && (row < m_items.size())) {
        beginRemoveRows(QModelIndex(), row, row);
        m_items.takeAt(row)->deleteLater();
        endRemoveRows();
    }
}

void YouTubePlaylistModel::onRequestFinished() {
    if (m_request->status() == QYouTube::ResourcesRequest::Ready) {
        const QVariantMap result = m_request->result().toMap();
        
        if (!result.isEmpty()) {
            m_nextPageToken = result.value("nextPageToken").toString();
            const QVariantList list = result.value("items").toList();

            if (!list.isEmpty()) {
                if (result.value("kind") == "youtube#searchListResponse") {
                    foreach (const QVariant &v, list) {
                        const QVariantMap item = v.toMap();
                        m_results << QPair<QString, QVariantMap>(YouTube::getPlaylistId(item), item);
                    }
                    
                    getAdditionalContent();
                }
                else {
                    beginInsertRows(QModelIndex(), m_items.size(), m_items.size() + list.size() - 1);
                    
                    foreach (const QVariant &item, list) {
                        m_items << new YouTubePlaylist(item.toMap(), this);
                    }
                    
                    endInsertRows();
                    emit countChanged(rowCount());
                    emit statusChanged(status());
                }

                return;
            }
        }
    }
    else {
        Logger::log("YouTubePlaylistModel::onRequestFinished(). Error: " + errorString());
    }

    emit statusChanged(status());
}

void YouTubePlaylistModel::onContentRequestFinished() {
    if (m_contentRequest->status() == QYouTube::ResourcesRequest::Ready) {
        const QVariantMap result = m_contentRequest->result().toMap();
        
        if (!result.isEmpty()) {
            const QVariantList list = result.value("items").toList();

            if (!list.isEmpty()) {
                beginInsertRows(QModelIndex(), m_items.size(), m_items.size() + list.size() - 1);
                
                foreach (const QVariant &v, list) {
                    const QVariantMap item = v.toMap();
                    
                    for (int i = 0; i < m_results.size(); i++) {
                        if (m_results.at(i).first == YouTube::getPlaylistId(item)) {
                            QVariantMap playlist = m_results.takeAt(i).second;
                            playlist["contentDetails"] = item.value("contentDetails");
                            m_items << new YouTubePlaylist(playlist, this);
                            break;
                        }
                    }
                }

                endInsertRows();
                emit countChanged(rowCount());
            }
        }
    }
    else {
        Logger::log("YouTubePlaylistModel::onContentRequestFinished(). Error: " + errorString());
    }

    emit statusChanged(status());
}

void YouTubePlaylistModel::onPlaylistCreated(YouTubePlaylist *playlist) {
    insert(0, new YouTubePlaylist(playlist, this));
}

void YouTubePlaylistModel::onPlaylistDeleted(YouTubePlaylist *playlist) {
    const QModelIndexList list = match(index(0), IdRole, playlist->id(), 1, Qt::MatchExactly);
    
    if (!list.isEmpty()) {
        remove(list.first().row());
    }
}
