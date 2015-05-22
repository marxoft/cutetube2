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

#ifndef VIMEOVIDEO_H
#define VIMEOVIDEO_H

#include "video.h"
#include <qvimeo/resourcesrequest.h>

class VimeoVideo : public Video
{
    Q_OBJECT
    
    Q_PROPERTY(QString errorString READ errorString NOTIFY statusChanged)
    Q_PROPERTY(bool favourited READ isFavourite NOTIFY favouriteChanged)
    Q_PROPERTY(qint64 favouriteCount READ favouriteCount NOTIFY favouriteCountChanged)
    Q_PROPERTY(QVimeo::ResourcesRequest::Status status READ status NOTIFY statusChanged)

public:
    explicit VimeoVideo(QObject *parent = 0);
    explicit VimeoVideo(const QString &id, QObject *parent = 0);
    explicit VimeoVideo(const QVariantMap &video, QObject *parent = 0);
    explicit VimeoVideo(const VimeoVideo *video, QObject *parent = 0);
    
    QString errorString() const;
    
    bool isFavourite() const;
    qint64 favouriteCount() const;
        
    QVimeo::ResourcesRequest::Status status() const;
    
    Q_INVOKABLE void loadVideo(const QString &id);
    Q_INVOKABLE void loadVideo(const QVariantMap &video);
    Q_INVOKABLE void loadVideo(VimeoVideo *video);
    
public Q_SLOTS:
    void favourite();
    void unfavourite();
    
    void watchLater();
    
private:
    void initRequest();
    
    void setFavourite(bool f);
    void setFavouriteCount(qint64 c);
        
private Q_SLOTS:
    void onVideoRequestFinished();
    void onFavouriteRequestFinished();
    void onUnfavouriteRequestFinished();
    void onWatchLaterRequestFinished();
    void onVideoUpdated(VimeoVideo *video);
    
Q_SIGNALS:
    void favouriteChanged();
    void favouriteCountChanged();
    void statusChanged(QVimeo::ResourcesRequest::Status s);

private:
    QVimeo::ResourcesRequest *m_request;
    
    bool m_favourite;
    qint64 m_favouriteCount;
};

#endif // VIMEOVIDEO_H
