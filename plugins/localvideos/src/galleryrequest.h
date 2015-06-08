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

#ifndef GALLERYREQUEST_H
#define GALLERYREQUEST_H

#include <QObject>
#include <QVariantMap>
#include <qmobilityglobal.h>

#if QT_VERSION >= 0x050000
QT_USE_DOCGALLERY_NAMESPACE
class QGalleryQueryRequest;
#else
QTM_BEGIN_NAMESPACE
class QGalleryQueryRequest;
class QDocumentGallery;
QTM_END_NAMESPACE

QTM_USE_NAMESPACE
#endif

class GalleryRequest : public QObject
{
    Q_OBJECT

public:
    explicit GalleryRequest(QObject *parent = 0);

    void getVideo(const QString &id);
    void listVideos(const QString &id);
    void searchVideos(const QString &query, const QString &order);

private Q_SLOTS:
    void onGetRequestFinished();
    void onListRequestFinished();

Q_SIGNALS:
    void finished();
    
private:
    QGalleryQueryRequest *m_request;
    QString m_filePath;
    QVariantMap m_filter;
};

#endif // GALLERYREQUEST_H
