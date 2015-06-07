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

#include "galleryrequest.h"
#include "json.h"
#include <QGalleryQueryRequest>
#include <QDocumentGallery>
#include <QDateTime>
#include <QUrl>
#include <QCryptographicHash>
#include <QSettings>
#include <QTimer>
#include <qplatformdefs.h>
#include <stdio.h>

static const int MAX_RESULTS = 20;

static const QStringList PROPERTY_NAMES = QStringList() << QDocumentGallery::duration << QDocumentGallery::fileName
                                                        << QDocumentGallery::filePath << QDocumentGallery::lastModified
                                                        << QDocumentGallery::playCount << QDocumentGallery::title;

#ifdef Q_WS_MAEMO_5
static const QByteArray THUMBNAIL_PATH("/home/user/.thumbnails/cropped/");
#elif defined MEEGO_EDITION_HARMATTAN
static const QByteArray THUMBNAIL_PATH("file:///home/user/.thumbnails/video-grid/");
#else
static const QByteArray THUMBNAIL_PATH();
#endif

inline static QString formatDuration(qint64 s) {
    return s > 0 ? QString("%1:%2").arg(s / 60, 2, 10, QChar('0')).arg(s % 60, 2, 10, QChar('0')) : QString("00:00");
}

inline static QByteArray formatThumbnail(const QByteArray &uri) {
    QCryptographicHash hash(QCryptographicHash::Md5);
    hash.addData(uri);
    return THUMBNAIL_PATH + hash.result().toHex() + ".jpeg";
}

GalleryRequest::GalleryRequest(QObject *parent) :
    QObject(parent),
    m_request(new QGalleryQueryRequest(this))
{
    m_request->setGallery(new QDocumentGallery(m_request));
    m_request->setRootType(QDocumentGallery::Video);
    m_request->setPropertyNames(PROPERTY_NAMES);
    m_request->setLimit(MAX_RESULTS);

    connect(m_request, SIGNAL(finished()), this, SLOT(onRequestFinished()));
}

void GalleryRequest::listVideos(const QString &id) {
    m_filter = QtJson::Json::parse(id).toMap();
    QGalleryUnionFilter ufilter;

    if (m_filter.isEmpty()) {
        QSettings settings("cuteTube2", "cuteTube2");
        QStringList paths = settings.value("Local videos/paths").toStringList();
        QString order = settings.value("Local videos/order", "title").toString();
        m_filter["paths"] = paths;
        m_filter["order"] = order;

        foreach (QString path, paths) {
            ufilter.append(QGalleryMetaDataFilter(QDocumentGallery::path, path, QGalleryFilter::StartsWith));
        }

        m_request->setSortPropertyNames(QStringList() << order);
    }
    else {
        QMapIterator<QString, QVariant> iterator(m_filter);

        while (iterator.hasNext()) {
            iterator.next();

            QString key = iterator.key();

            if (key == "offset") {
                m_request->setOffset(iterator.value().toInt());
            }
            else if (key == "order") {
                m_request->setSortPropertyNames(QStringList() << iterator.value().toString());
            }
            else if (key == "paths") {
                foreach (QString path, iterator.value().toStringList()) {
                    ufilter.append(QGalleryMetaDataFilter(QDocumentGallery::path, path, QGalleryFilter::StartsWith));
                }
            }
            else {
                ufilter.append(QGalleryMetaDataFilter(iterator.key(), iterator.value(), QGalleryFilter::Contains));
            }
        }
    }

    m_request->setFilter(ufilter);
    QTimer::singleShot(500, m_request, SLOT(execute()));
}

void GalleryRequest::searchVideos(const QString &query, const QString &order) {
    m_filter = QVariantMap();
    m_filter["title"] = query;
    m_filter["order"] = order;

    m_request->setFilter(QGalleryMetaDataFilter(QDocumentGallery::title, query, QGalleryFilter::Contains));
    m_request->setSortPropertyNames(QStringList() << order);
    QTimer::singleShot(500, m_request, SLOT(execute()));
}

void GalleryRequest::onRequestFinished() {
    QVariantMap result;
    QVariantList items;
    QVariantMap f = m_filter;
    f.remove("offset");
    QByteArray id = QtJson::Json::serialize(f);

    const int count = m_request->itemCount();

    for (int i = 0; i < count; i++) {
        if (m_request->isValid()) {
            QVariantMap item;
            QString filePath = m_request->metaData(QDocumentGallery::filePath).toString();
            QByteArray thumbnailUrl = formatThumbnail("file://" + filePath.toUtf8().replace(" ", "%20"));
            QString title = m_request->metaData(QDocumentGallery::title).toString();

            item["date"] = m_request->metaData(QDocumentGallery::lastModified).toDateTime().toString("dd MMM yyyy");
            item["duration"] = formatDuration(m_request->metaData(QDocumentGallery::duration).toLongLong());
            item["id"] = id;
            item["largeThumbnailUrl"] = thumbnailUrl;
            item["streamUrl"] = "file://" + filePath;
            item["thumbnailUrl"] = thumbnailUrl;
            item["title"] = title.isEmpty() ? m_request->metaData(QDocumentGallery::fileName).toString().section('.', 0, -2)
                                            : title;
            item["url"] = filePath;
            item["viewCount"] = m_request->metaData(QDocumentGallery::playCount).toLongLong();
            items << item;
        }

        m_request->next();
    }

    if (count > 0) {
        f["offset"] = m_request->offset() + count;
        result["next"] = QtJson::Json::serialize(f);
    }

    result["items"] = items;
    printf(QtJson::Json::serialize(result).constData());
    emit finished();
}
