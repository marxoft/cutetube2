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

#include "imagecache.h"
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QThread>

class CachedImage
{

public:
    explicit CachedImage(const QUrl &u) :
        url(u)
    {
    }
    
    const QUrl url;
    QImage image;    
};

QThread* ImageCache::thread = 0;
QQueue<QUrl> ImageCache::queue;
QCache<QUrl, CachedImage> ImageCache::cache;

int ImageCache::requestCount = 0;
int ImageCache::refCount = 0;

static const int MAX_REQUESTS = 8;

ImageCache::ImageCache() :
    QObject(),
    m_manager(new QNetworkAccessManager)
{
    refCount++;
    
    if (!thread) {
        thread = new QThread;
        thread->start();
    }
    
    moveToThread(thread);
}

ImageCache::~ImageCache() {
    delete m_manager;
    m_manager = 0;
    refCount--;
    
    if (refCount == 0) {
        thread->quit();
        thread->deleteLater();
        thread = 0;
    }
}

QImage ImageCache::image(const QUrl &url, const QSize &size) {
    if (CachedImage *ci = cache.object(url)) {
        return (!size.isEmpty())
               && (!ci->image.isNull()) ? ci->image.scaled(size, Qt::IgnoreAspectRatio, Qt::SmoothTransformation)
                                        : ci->image;
    }
    
    cache.insert(url, new CachedImage(url));
    
    if (requestCount < MAX_REQUESTS) {
        getImage(url);
    }
    else {
        queue.enqueue(url);
    }
    
    return QImage();
}

void ImageCache::getImage(const QUrl &url) {
    requestCount++;
    ImageRequest *request = new ImageRequest(m_manager, url);
    connect(request, SIGNAL(finished(ImageRequest*)), this, SLOT(onRequestFinished(ImageRequest*)));
}

void ImageCache::onRequestFinished(ImageRequest *request) {    
    if (CachedImage *ci = cache.object(request->url)) {
        QImage image;
        image.loadFromData(request->reply->readAll());
        ci->image = image;
        emit imageReady();
    }
    
    request->deleteLater();
    requestCount--;
    
    if ((!queue.isEmpty()) && (requestCount < MAX_REQUESTS)) {
        getImage(queue.dequeue());
    }
}

ImageRequest::ImageRequest(QNetworkAccessManager *manager, const QUrl &u) :
    QObject()
{
    url = u;
    reply = manager->get(QNetworkRequest(url));
    connect(reply, SIGNAL(finished()), this, SLOT(onReplyFinished()));
}

ImageRequest::~ImageRequest() {
    reply->deleteLater();
    reply = 0;
}

void ImageRequest::onReplyFinished() {
    emit finished(this);
}
