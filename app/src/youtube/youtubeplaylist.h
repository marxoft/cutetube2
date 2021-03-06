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

#ifndef YOUTUBEPLAYLIST_H
#define YOUTUBEPLAYLIST_H

#include "playlist.h"
#include <qyoutube/resourcesrequest.h>
#include <QPointer>

class YouTubeVideo;

class YouTubePlaylist : public CTPlaylist
{
    Q_OBJECT
    
    Q_PROPERTY(QString errorString READ errorString NOTIFY statusChanged)
    Q_PROPERTY(QString privacyStatus READ privacyStatus NOTIFY privacyStatusChanged)
    Q_PROPERTY(QYouTube::ResourcesRequest::Status status READ status NOTIFY statusChanged)

public:
    explicit YouTubePlaylist(QObject *parent = 0);
    explicit YouTubePlaylist(const QString &id, QObject *parent = 0);
    explicit YouTubePlaylist(const QVariantMap &playlist, QObject *parent = 0);
    explicit YouTubePlaylist(const YouTubePlaylist *playlist, QObject *parent = 0);
    
    QString errorString() const;
    
    QString privacyStatus() const;
    
    QYouTube::ResourcesRequest::Status status() const;
    
    Q_INVOKABLE void loadPlaylist(const QString &id);
    Q_INVOKABLE void loadPlaylist(const QVariantMap &playlist);
    Q_INVOKABLE void loadPlaylist(YouTubePlaylist *playlist);
    
public Q_SLOTS:
    void addVideo(YouTubeVideo *video);
    void addVideo(const QVariantMap &playlist, YouTubeVideo *video);
    void removeVideo(YouTubeVideo *video);
    
private:
    void initRequest();
    
    void setPrivacyStatus(const QString &s);
            
private Q_SLOTS:
    void onPlaylistRequestFinished();
    void onCreatePlaylistRequestFinished();
    void onAddVideoRequestFinished();
    void onRemoveVideoRequestFinished();
    void onPlaylistUpdated(YouTubeVideo*, YouTubePlaylist*);
    
Q_SIGNALS:
    void privacyStatusChanged();
    void statusChanged(QYouTube::ResourcesRequest::Status s);

private:
    QYouTube::ResourcesRequest *m_request;
    QPointer<YouTubeVideo> m_video;
    
    QString m_privacyStatus;
};

#endif // YOUTUBEPLAYLIST_H
