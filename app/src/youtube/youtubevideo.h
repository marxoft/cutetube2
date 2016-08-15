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

#ifndef YOUTUBEVIDEO_H
#define YOUTUBEVIDEO_H

#include "video.h"
#include <qyoutube/resourcesrequest.h>

class YouTubeVideo : public CTVideo
{
    Q_OBJECT
    
    Q_PROPERTY(bool disliked READ isDisliked NOTIFY dislikedChanged)
    Q_PROPERTY(qint64 dislikeCount READ dislikeCount NOTIFY dislikeCountChanged)
    Q_PROPERTY(QString errorString READ errorString NOTIFY statusChanged)
    Q_PROPERTY(bool favourited READ isFavourite NOTIFY favouriteChanged)
    Q_PROPERTY(qint64 favouriteCount READ favouriteCount NOTIFY favouriteCountChanged)
    Q_PROPERTY(QString favouriteId READ favouriteId NOTIFY favouriteIdChanged)
    Q_PROPERTY(bool liked READ isLiked NOTIFY likedChanged)
    Q_PROPERTY(qint64 likeCount READ likeCount NOTIFY likeCountChanged)
    Q_PROPERTY(QString playlistItemId READ playlistItemId NOTIFY playlistItemIdChanged)
    Q_PROPERTY(QYouTube::ResourcesRequest::Status status READ status NOTIFY statusChanged)

public:
    explicit YouTubeVideo(QObject *parent = 0);
    explicit YouTubeVideo(const QString &id, QObject *parent = 0);
    explicit YouTubeVideo(const QVariantMap &video, QObject *parent = 0);
    explicit YouTubeVideo(const YouTubeVideo *video, QObject *parent = 0);
    
    bool isDisliked() const;
    qint64 dislikeCount() const;
    
    QString errorString() const;
    
    bool isFavourite() const;
    qint64 favouriteCount() const;
    QString favouriteId() const;
    
    bool isLiked() const;
    qint64 likeCount() const;
    
    QString playlistItemId() const;
    
    QYouTube::ResourcesRequest::Status status() const;
    
    Q_INVOKABLE void loadVideo(const QString &id);
    Q_INVOKABLE void loadVideo(const QVariantMap &video);
    Q_INVOKABLE void loadVideo(YouTubeVideo *video);
    
public Q_SLOTS:
    void favourite();
    void unfavourite();
    
    void like();
    void dislike();
    
private:
    void initRequest();
    
    void setDisliked(bool d);
    void setDislikeCount(qint64 c);
    
    void setFavourite(bool f);
    void setFavouriteCount(qint64 c);
    void setFavouriteId(const QString &i);
    
    void setLiked(bool l);
    void setLikeCount(qint64 c);
    
    void setPlaylistItemId(const QString &i);
        
private Q_SLOTS:
    void onVideoRequestFinished();
    void onFavouriteRequestFinished();
    void onUnfavouriteRequestFinished();
    void onLikeRequestFinished();
    void onDislikeRequestFinished();
    void onVideoUpdated(YouTubeVideo *video);
    
Q_SIGNALS:
    void dislikedChanged();
    void dislikeCountChanged();
    void favouriteChanged();
    void favouriteCountChanged();
    void favouriteIdChanged();
    void likedChanged();
    void likeCountChanged();
    void playlistItemIdChanged();
    void statusChanged(QYouTube::ResourcesRequest::Status s);

private:
    QYouTube::ResourcesRequest *m_request;
    
    bool m_disliked;
    qint64 m_dislikeCount;
    bool m_favourite;
    qint64 m_favouriteCount;
    QString m_favouriteId;
    bool m_liked;    
    qint64 m_likeCount;
    QString m_playlistItemId;
};

#endif // YOUTUBEVIDEO_H
