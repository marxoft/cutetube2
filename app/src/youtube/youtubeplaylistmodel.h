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

#ifndef YOUTUBEPLAYLISTMODEL_H
#define YOUTUBEPLAYLISTMODEL_H

#include "youtubeplaylist.h"
#include <QAbstractListModel>
#include <QStringList>

class YouTubePlaylistModel : public QAbstractListModel
{
    Q_OBJECT
    
    Q_PROPERTY(bool canFetchMore READ canFetchMore NOTIFY statusChanged)
    Q_PROPERTY(int count READ rowCount NOTIFY countChanged)
    Q_PROPERTY(QString errorString READ errorString NOTIFY statusChanged)
    Q_PROPERTY(QYouTube::ResourcesRequest::Status status READ status NOTIFY statusChanged)
    
public:
    enum Roles {
        DateRole = Qt::UserRole + 1,
        DescriptionRole,
        IdRole,
        LargeThumbnailUrlRole,
        ThumbnailUrlRole,
        TitleRole,
        UserIdRole,
        UsernameRole,
        VideoCountRole
    };
    
    explicit YouTubePlaylistModel(QObject *parent = 0);
    
    QString errorString() const;
    
    QYouTube::ResourcesRequest::Status status() const;
    
#if QT_VERSION >= 0x050000
    QHash<int, QByteArray> roleNames() const;
#endif
    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    
    bool canFetchMore(const QModelIndex &parent = QModelIndex()) const;
    Q_INVOKABLE void fetchMore(const QModelIndex &parent = QModelIndex());
    
    QVariant data(const QModelIndex &index, int role) const;
    QMap<int, QVariant> itemData(const QModelIndex &index) const;
    
    Q_INVOKABLE QVariant data(int row, const QByteArray &role) const;
    Q_INVOKABLE QVariantMap itemData(int row) const;
    
    Q_INVOKABLE YouTubePlaylist* get(int row) const;
    
    Q_INVOKABLE void list(const QString &resourcePath, const QStringList &part,
                          const QVariantMap &filters = QVariantMap(), const QVariantMap &params = QVariantMap());

public Q_SLOTS:
    void clear();
    void cancel();
    void reload();
    
private:
    void getAdditionalContent();
    void loadResults();
    
    void append(YouTubePlaylist *playlist);
    void insert(int row, YouTubePlaylist *playlist);
    void remove(int row);
    
private Q_SLOTS:
    void onRequestFinished();
    void onContentRequestFinished();
    void onPlaylistCreated(YouTubePlaylist *playlist);
    void onPlaylistDeleted(YouTubePlaylist *playlist);
    
Q_SIGNALS:
    void countChanged(int c);
    void statusChanged(QYouTube::ResourcesRequest::Status s);
    
private:
    QYouTube::ResourcesRequest *m_request;
    QYouTube::ResourcesRequest *m_contentRequest;
    
    QString m_resourcePath;
    QStringList m_part;
    QVariantMap m_filters;
    QVariantMap m_params;
    QString m_nextPageToken;
    
    QVariantList m_results;
    
    QList<YouTubePlaylist*> m_items;
    
    QHash<int, QByteArray> m_roles;
};
    
#endif // YOUTUBEPLAYLISTMODEL_H
