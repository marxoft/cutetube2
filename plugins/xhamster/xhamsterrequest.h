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

#ifndef XHAMSTERREQUEST_H
#define XHAMSTERREQUEST_H

#include "resourcesrequest.h"
#include <QRegExp>

class QNetworkAccessManager;
class QNetworkReply;

class XHamsterRequest : public ResourcesRequest
{
    Q_OBJECT

public:
    explicit XHamsterRequest(QObject *parent = 0);

    virtual QString errorString() const;

    virtual QVariant result() const;

    virtual ResourcesRequest::Status status() const;
    
public Q_SLOTS:
    virtual bool cancel();
    virtual bool get(const QString &resourceType, const QString &resourceId);
    virtual bool list(const QString &resourceType, const QString &resourceId);
    virtual bool search(const QString &resourceType, const QString &query, const QString &order);

private Q_SLOTS:
    void checkVideo();
    void checkVideos();
    
    void checkCategories();
    
    void checkStreams();

private:
    static QString getRedirect(const QNetworkReply *reply);
    
    static QUrl incrementPageNumber(QUrl url);
    
    void setErrorString(const QString &e);

    void setResult(const QVariant &r);

    void setStatus(ResourcesRequest::Status s);
    
    void getVideo(const QString &url);
    void listVideos(const QString &url);
    void searchVideos(const QString &query, const QString &order);
        
    void listCategories(const QString &url);
    
    void listStreams(const QString &url);
    
    void followRedirect(const QString &url, const char *slot);
    
    QNetworkAccessManager* networkAccessManager();

    static const QString BASE_URL;

    static const QByteArray USER_AGENT;
    
    static const QRegExp PAGE_REGEXP;
        
    static const int MAX_REDIRECTS;    
    
    QNetworkAccessManager *m_nam;

    QString m_errorString;

    QVariant m_result;

    Status m_status;
    
    int m_redirects;
};

#endif // XHAMSTERREQUEST_H
