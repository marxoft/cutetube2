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

#ifndef DAILYMOTIONPLAYLIST_H
#define DAILYMOTIONPLAYLIST_H

#include "playlist.h"
#include <qdailymotion/resourcesrequest.h>
#include <QPointer>

class DailymotionVideo;

class DailymotionPlaylist : public CTPlaylist
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
    void addVideo(DailymotionVideo *video);
    void addVideo(const QVariantMap &playlist, DailymotionVideo *video);
    void removeVideo(DailymotionVideo *video);
    
private:
    void initRequest();
            
private Q_SLOTS:
    void onPlaylistRequestFinished();
    void onCreatePlaylistRequestFinished();
    void onAddVideoRequestFinished();
    void onRemoveVideoRequestFinished();
    void onPlaylistUpdated(DailymotionVideo*, DailymotionPlaylist*);
    
Q_SIGNALS:
    void statusChanged(QDailymotion::ResourcesRequest::Status s);

private:
    QDailymotion::ResourcesRequest *m_request;
    QPointer<DailymotionVideo> m_video;
};

#endif // DAILYMOTIONPLAYLIST_H
