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

#ifndef IMAGECACHE_H
#define IMAGECACHE_H

#include <QQueue>
#include <QCache>
#include <QImage>
#include <QUrl>

class CachedImage;
class ImageRequest;
class QThread;
class QNetworkAccessManager;
class QNetworkReply;

class ImageCache : public QObject
{
    Q_OBJECT
    
public:
    explicit ImageCache();
    ~ImageCache();
    
    QImage image(const QUrl &url, const QSize &size = QSize());
    
private Q_SLOTS:
    void onRequestFinished(ImageRequest *request);
    
Q_SIGNALS:
    void imageReady();
    
private:
    void getImage(const QUrl &url);
    
    static QThread *thread;
    
    static QQueue<QUrl> queue;
    static QCache<QUrl, CachedImage> cache;
    
    static int requestCount;
    static int refCount;
    
    QNetworkAccessManager *m_manager;
};

class ImageRequest : public QObject
{
    Q_OBJECT
    
private:
    ImageRequest(QNetworkAccessManager *manager, const QUrl &u);
    ~ImageRequest();
    
    QNetworkReply *reply;
    QUrl url;
    
    friend class ImageCache;
    
private Q_SLOTS:
    void onReplyFinished();
    
Q_SIGNALS:
    void finished(ImageRequest *request);
};
    
#endif // IMAGECACHE_H
