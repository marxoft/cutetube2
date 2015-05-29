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

#ifndef DAILYMOTIONVIDEO_H
#define DAILYMOTIONVIDEO_H

#include "video.h"
#include <qdailymotion/resourcesrequest.h>

class DailymotionVideo : public CTVideo
{
    Q_OBJECT
    
    Q_PROPERTY(QString errorString READ errorString NOTIFY statusChanged)
    Q_PROPERTY(bool favourited READ isFavourite NOTIFY favouriteChanged)
    Q_PROPERTY(QDailymotion::ResourcesRequest::Status status READ status NOTIFY statusChanged)

public:
    explicit DailymotionVideo(QObject *parent = 0);
    explicit DailymotionVideo(const QString &id, QObject *parent = 0);
    explicit DailymotionVideo(const QVariantMap &video, QObject *parent = 0);
    explicit DailymotionVideo(const DailymotionVideo *video, QObject *parent = 0);
    
    QString errorString() const;
    
    bool isFavourite() const;
        
    QDailymotion::ResourcesRequest::Status status() const;
    
    Q_INVOKABLE void loadVideo(const QString &id);
    Q_INVOKABLE void loadVideo(const QVariantMap &video);
    Q_INVOKABLE void loadVideo(DailymotionVideo *video);
    
public Q_SLOTS:
    void favourite();
    void unfavourite();
    
private:
    void initRequest();
    
    void setFavourite(bool f);
        
private Q_SLOTS:
    void onVideoRequestFinished();
    void onFavouriteRequestFinished();
    void onUnfavouriteRequestFinished();
    void onVideoUpdated(DailymotionVideo *video);
    
Q_SIGNALS:
    void favouriteChanged();
    void statusChanged(QDailymotion::ResourcesRequest::Status s);

private:
    QDailymotion::ResourcesRequest *m_request;
    
    bool m_favourite;
};

#endif // DAILYMOTIONVIDEO_H
