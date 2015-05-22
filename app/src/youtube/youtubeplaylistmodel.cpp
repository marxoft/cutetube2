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

#include "youtubeplaylistmodel.h"
#include "youtube.h"
#ifdef CUTETUBE_DEBUG
#include <QDebug>
#endif

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
    m_request->setApiKey(YouTube::instance()->apiKey());
    m_request->setClientId(YouTube::instance()->clientId());
    m_request->setClientSecret(YouTube::instance()->clientSecret());
    m_request->setAccessToken(YouTube::instance()->accessToken());
    m_request->setRefreshToken(YouTube::instance()->refreshToken());
    
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
    if (YouTubePlaylist *playlist = get(index.row())) {
        return playlist->property(m_roles[role]);
    }
    
    return QVariant();
}

QMap<int, QVariant> YouTubePlaylistModel::itemData(const QModelIndex &index) const {
    QMap<int, QVariant> map;
    
    if (YouTubePlaylist *playlist = get(index.row())) {
        QHashIterator<int, QByteArray> iterator(m_roles);
        
        while (iterator.hasNext()) {
            iterator.next();
            map[iterator.key()] = playlist->property(iterator.value());
        }
    }
    
    return map;
}

QVariant YouTubePlaylistModel::data(int row, const QByteArray &role) const {
    if (YouTubePlaylist *playlist = get(row)) {
        return playlist->property(role);
    }
    
    return QVariant();
}

QVariantMap YouTubePlaylistModel::itemData(int row) const {
    QVariantMap map;
    
    if (YouTubePlaylist *playlist = get(row)) {
        foreach (QByteArray role, m_roles.values()) {
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
    
    clear();
    m_resourcePath = resourcePath;
    m_part = part;
    m_filters = filters;
    m_params = params;
    m_request->list(resourcePath, part, filters, params);
    emit statusChanged(status());
    
    disconnect(YouTube::instance(), 0, this, 0);
    
    if (filters.value("channelId") == YouTube::instance()->userId()) {
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
    clear();
    m_request->list(m_resourcePath, m_part, m_filters, m_params);
    emit statusChanged(status());
}

void YouTubePlaylistModel::getAdditionalContent() {
    if (!m_contentRequest) {
        m_contentRequest = new QYouTube::ResourcesRequest(this);
        m_contentRequest->setApiKey(YouTube::instance()->apiKey());
        m_contentRequest->setClientId(YouTube::instance()->clientId());
        m_contentRequest->setClientSecret(YouTube::instance()->clientSecret());
        m_contentRequest->setAccessToken(YouTube::instance()->accessToken());
        m_contentRequest->setRefreshToken(YouTube::instance()->refreshToken());

        connect(m_contentRequest, SIGNAL(accessTokenChanged(QString)),
                YouTube::instance(), SLOT(setAccessToken(QString)));
        connect(m_contentRequest, SIGNAL(refreshTokenChanged(QString)),
                YouTube::instance(), SLOT(setRefreshToken(QString)));
        connect(m_contentRequest, SIGNAL(finished()), this, SLOT(onContentRequestFinished()));
    }
    
    QStringList ids;
    
    foreach (QVariant result, m_results) {
        ids << YouTube::getPlaylistId(result.toMap());
    }
    
    QVariantMap filters;
    filters["id"] = ids.join(",");
    
    m_contentRequest->list("/playlists", QStringList() << "contentDetails", filters);
}

void YouTubePlaylistModel::loadResults() {
    beginInsertRows(QModelIndex(), m_items.size(), m_items.size() + m_results.size() - 1);
    
    foreach (QVariant result, m_results) {
        m_items << new YouTubePlaylist(result.toMap(), this);
    }

    endInsertRows();
    emit countChanged(rowCount());
    emit statusChanged(status());
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
        QVariantMap result = m_request->result().toMap();
        
        if (!result.isEmpty()) {
            m_nextPageToken = result.value("nextPageToken").toString();
            m_results = result.value("items").toList();

            if (!m_results.isEmpty()) {
                if (result.value("kind") == "youtube#searchListResponse") {
                    getAdditionalContent();
                }
                else {
                    loadResults();
                }

                return;
            }
        }
    }

    emit statusChanged(status());
}

void YouTubePlaylistModel::onContentRequestFinished() {
    if (m_contentRequest->status() == QYouTube::ResourcesRequest::Ready) {
        QVariantMap result = m_contentRequest->result().toMap();
        
        if (!result.isEmpty()) {
            QVariantList list = result.value("items").toList();
            const int count = qMin(list.size(), m_results.size());
            
            for (int i = 0; i < count; i++) {
                QVariantMap item = list.at(i).toMap();
                QVariantMap playlist = m_results.takeAt(i).toMap();
                playlist["contentDetails"] = item.value("contentDetails");
                m_results.insert(i, playlist);
            }
        }
    }

    loadResults();
}

void YouTubePlaylistModel::onPlaylistCreated(YouTubePlaylist *playlist) {
    insert(0, new YouTubePlaylist(playlist, this));
#ifdef CUTETUBE_DEBUG
    qDebug() << "YouTubePlaylistModel::onPlaylistCreated" << playlist->title();
#endif
}

void YouTubePlaylistModel::onPlaylistDeleted(YouTubePlaylist *playlist) {
    QModelIndexList list = match(index(0), IdRole, playlist->id(), 1, Qt::MatchExactly);
    
    if (!list.isEmpty()) {
        remove(list.first().row());
    }
#ifdef CUTETUBE_DEBUG
    qDebug() << "YouTubePlaylistModel::onPlaylistDeleted" << playlist->id();
#endif
}
