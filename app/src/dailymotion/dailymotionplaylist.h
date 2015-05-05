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

#ifndef DAILYMOTIONPLAYLIST_H
#define DAILYMOTIONPLAYLIST_H

#include "playlist.h"
#include <qdailymotion/resourcesrequest.h>

class DailymotionVideo;

class DailymotionPlaylist : public Playlist
{
    Q_OBJECT
    
    Q_PROPERTY(QString errorString READ errorString NOTIFY statusChanged)
    Q_PROPERTY(QDailymotion::ResourcesRequest::Status status READ status NOTIFY statusChanged)

public:
    explicit DailymotionPlaylist(QObject *parent = 0);
    explicit DailymotionPlaylist(const QString &id, QObject *parent = 0);
    explicit DailymotionPlaylist(const QVariantMap &playlist, QObject *parent = 0);
    explicit DailymotionPlaylist(const DailymotionPlaylist *playlist, QObject *parent = 0);
    
    QString errorString() const;
        
    QDailymotion::ResourcesRequest::Status status() const;
    
    Q_INVOKABLE void loadPlaylist(const QString &id);
    Q_INVOKABLE void loadPlaylist(const QVariantMap &playlist);
    
public Q_SLOTS:
    void addVideo(const DailymotionVideo *video);
    void removeVideo(const DailymotionVideo *video);
    
private:
    void initRequest();
            
private Q_SLOTS:
    void onPlaylistRequestFinished();
    void onCreatePlaylistRequestFinished();
    void onAddVideoRequestFinished();
    void onRemoveVideoRequestFinished();
    void onPlaylistUpdated(const DailymotionVideo*, const DailymotionPlaylist*);
    
Q_SIGNALS:
    void statusChanged(QDailymotion::ResourcesRequest::Status s);

private:
    QDailymotion::ResourcesRequest *m_request;
    const DailymotionVideo *m_video;
};

#endif // DAILYMOTIONPLAYLIST_H
