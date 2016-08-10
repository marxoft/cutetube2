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

#include "vimeovideomodel.h"
#include "vimeo.h"
#include "vimeoplaylist.h"

VimeoVideoModel::VimeoVideoModel(QObject *parent) :
    QAbstractListModel(parent),
    m_request(new QVimeo::ResourcesRequest(this))
{
    m_roles[DateRole] = "date";
    m_roles[DescriptionRole] = "description";
    m_roles[DownloadableRole] = "downloadable";
    m_roles[DurationRole] = "duration";
    m_roles[FavouriteRole] = "favourited";
    m_roles[IdRole] = "id";
    m_roles[LargeThumbnailUrlRole] = "largeThumbnailUrl";
    m_roles[StreamUrlRole] = "streamUrl";
    m_roles[SubtitlesRole] = "subtitles";
    m_roles[ThumbnailUrlRole] = "thumbnailUrl";
    m_roles[TitleRole] = "title";
    m_roles[UrlRole] = "url";
    m_roles[UserIdRole] = "userId";
    m_roles[UsernameRole] = "username";
    m_roles[ViewCountRole] = "viewCount";
#if QT_VERSION < 0x050000
    setRoleNames(m_roles);
#endif
    m_request->setClientId(Vimeo::clientId());
    m_request->setClientSecret(Vimeo::clientSecret());
    m_request->setAccessToken(Vimeo::accessToken());
    
    connect(m_request, SIGNAL(accessTokenChanged(QString)), Vimeo::instance(), SLOT(setAccessToken(QString)));
    connect(m_request, SIGNAL(finished()), this, SLOT(onRequestFinished()));
}

QString VimeoVideoModel::errorString() const {
    return Vimeo::getErrorString(m_request->result().toMap());
}

QVimeo::ResourcesRequest::Status VimeoVideoModel::status() const {
    return m_request->status();
}

#if QT_VERSION >=0x050000
QHash<int, QByteArray> VimeoVideoModel::roleNames() const {
    return m_roles;
}
#endif

int VimeoVideoModel::rowCount(const QModelIndex &) const {
    return m_items.size();
}

bool VimeoVideoModel::canFetchMore(const QModelIndex &) const {
    return (status() != QVimeo::ResourcesRequest::Loading) && (m_hasMore);
}

void VimeoVideoModel::fetchMore(const QModelIndex &) {
    if (!canFetchMore()) {
        return;
    }
    
    const int page = m_filters.value("page").toInt();
    m_filters["page"] = (page > 0 ? page + 1 : 2);
    m_request->list(m_resourcePath, m_filters);
    emit statusChanged(status());
}

QVariant VimeoVideoModel::data(const QModelIndex &index, int role) const {
    if (const VimeoVideo *video = get(index.row())) {
        return video->property(m_roles[role]);
    }
    
    return QVariant();
}

QMap<int, QVariant> VimeoVideoModel::itemData(const QModelIndex &index) const {
    QMap<int, QVariant> map;
    
    if (const VimeoVideo *video = get(index.row())) {
        QHashIterator<int, QByteArray> iterator(m_roles);
        
        while (iterator.hasNext()) {
            iterator.next();
            map[iterator.key()] = video->property(iterator.value());
        }
    }
    
    return map;
}

QVariant VimeoVideoModel::data(int row, const QByteArray &role) const {
    if (const VimeoVideo *video = get(row)) {
        return video->property(role);
    }
    
    return QVariant();
}

QVariantMap VimeoVideoModel::itemData(int row) const {
    QVariantMap map;
    
    if (const VimeoVideo *video = get(row)) {
        foreach (const QByteArray &role, m_roles.values()) {
            map[role] = video->property(role);
        }
    }
    
    return map;
}

VimeoVideo* VimeoVideoModel::get(int row) const {
    if ((row >= 0) && (row < m_items.size())) {
        return m_items.at(row);
    }
    
    return 0;
}

void VimeoVideoModel::list(const QString &resourcePath, const QVariantMap &filters) {
    if (status() == QVimeo::ResourcesRequest::Loading) {
        return;
    }
    
    clear();
    m_resourcePath = resourcePath;
    m_filters = filters;
    m_request->list(resourcePath, filters);
    emit statusChanged(status());
    
    disconnect(Vimeo::instance(), 0, this, 0);
        
    if (resourcePath == "/me/likes") {
        connect(Vimeo::instance(), SIGNAL(videoFavourited(VimeoVideo*)),
                this, SLOT(onVideoFavourited(VimeoVideo*)));
        connect(Vimeo::instance(), SIGNAL(videoUnfavourited(VimeoVideo*)),
                this, SLOT(onVideoUnfavourited(VimeoVideo*)));
    }
    else if (resourcePath == "/me/watchlater") {
        connect(Vimeo::instance(), SIGNAL(videoWatchLater(VimeoVideo*)),
                this, SLOT(onVideoWatchLater(VimeoVideo*)));
    }
    else if (resourcePath.section('/', -3, -3) == "playlists") {
        connect(Vimeo::instance(), SIGNAL(videoAddedToPlaylist(VimeoVideo*, VimeoPlaylist*)),
                this, SLOT(onVideoAddedToPlaylist(VimeoVideo*, VimeoPlaylist*)));
        connect(Vimeo::instance(), SIGNAL(videoRemovedFromPlaylist(VimeoVideo*, VimeoPlaylist*)),
                this, SLOT(onVideoRemovedFromPlaylist(VimeoVideo*, VimeoPlaylist*)));
    }
}

void VimeoVideoModel::clear() {
    if (!m_items.isEmpty()) {
        beginResetModel();
        qDeleteAll(m_items);
        m_items.clear();
        m_hasMore = false;
        endResetModel();
        emit countChanged(rowCount());
    }
}

void VimeoVideoModel::cancel() {
    m_request->cancel();
}

void VimeoVideoModel::reload() {
    clear();
    m_request->list(m_resourcePath, m_filters);
    emit statusChanged(status());
}

void VimeoVideoModel::append(VimeoVideo *video) {
    beginInsertRows(QModelIndex(), m_items.size(), m_items.size());
    m_items << video;
    endInsertRows();
}

void VimeoVideoModel::insert(int row, VimeoVideo *video) {
    if ((row >= 0) && (row < m_items.size())) {
        beginInsertRows(QModelIndex(), row, row);
        m_items.insert(row, video);
        endInsertRows();
    }
    else {
        append(video);
    }
}

void VimeoVideoModel::remove(int row) {
    if ((row >= 0) && (row < m_items.size())) {
        beginRemoveRows(QModelIndex(), row, row);
        m_items.takeAt(row)->deleteLater();
        endRemoveRows();
    }
}

void VimeoVideoModel::onRequestFinished() {
    if (m_request->status() == QVimeo::ResourcesRequest::Ready) {
        const QVariantMap result = m_request->result().toMap();
        
        if (!result.isEmpty()) {
            m_hasMore = !result.value("paging").toMap().value("next").isNull();
            const QVariantList list = result.value("data").toList();

            beginInsertRows(QModelIndex(), m_items.size(), m_items.size() + list.size() - 1);
    
            if (m_resourcePath.endsWith("/feed")) {
                foreach (const QVariant &item, list) {
                    m_items << new VimeoVideo(item.toMap().value("clip").toMap(), this);
                }
            }
            else {
                foreach (const QVariant &item, list) {
                    m_items << new VimeoVideo(item.toMap(), this);
                }
            }

            endInsertRows();
            emit countChanged(rowCount());
        }
    }
    
    emit statusChanged(status());
}

void VimeoVideoModel::onVideoAddedToPlaylist(VimeoVideo *video, VimeoPlaylist *playlist) {
    if (m_resourcePath.section('/', -2, -2) == playlist->id()) {
        insert(0, new VimeoVideo(video, this));
    }
}

void VimeoVideoModel::onVideoRemovedFromPlaylist(VimeoVideo *video, VimeoPlaylist *playlist) {
    if (m_resourcePath.section('/', -2, -2) == playlist->id()) {
        const QModelIndexList list = match(index(0), IdRole, video->id(), 1, Qt::MatchExactly);
        
        if (!list.isEmpty()) {
            remove(list.first().row());
        }
    }
}

void VimeoVideoModel::onVideoFavourited(VimeoVideo *video) {
    insert(0, new VimeoVideo(video, this));
}

void VimeoVideoModel::onVideoUnfavourited(VimeoVideo *video) {
    const QModelIndexList list = match(index(0), IdRole, video->id(), 1, Qt::MatchExactly);
    
    if (!list.isEmpty()) {
        remove(list.first().row());
    }
}

void VimeoVideoModel::onVideoWatchLater(VimeoVideo *video) {
    insert(0, new VimeoVideo(video, this));
}
