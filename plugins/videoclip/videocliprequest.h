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

#ifndef VIDEOCLIPREQUEST_H
#define VIDEOCLIPREQUEST_H

#include "resourcesrequest.h"
#include <QRegExp>

class QNetworkAccessManager;
class QNetworkReply;

namespace QYouTube {
    class StreamsRequest;
}

class VideoclipRequest : public ResourcesRequest
{
    Q_OBJECT

public:
    explicit VideoclipRequest(QObject *parent = 0);

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
    void checkRelatedVideos();
    void checkUserVideos();
    
    void checkComments();
    
    void checkUser();
    void checkUsers();
    
    void checkStreams();
    void checkYouTubeStreams();

private:
    static QString getRedirect(const QNetworkReply *reply);
    
    void setErrorString(const QString &e);

    void setResult(const QVariant &r);

    void setStatus(ResourcesRequest::Status s);
    
    void getVideo(const QString &id);
    void listVideos(const QString &url);
    void listRelatedVideos(const QString &id);
    void listUserVideos(const QString &id);
    void searchVideos(const QString &query, const QString &order);

    void listComments(const QString &id);
        
    void getUser(const QString &id);
    void listUsers(const QString &url);
        
    void listStreams(const QString &id);
    
    void followRedirect(const QString &url, const char *slot);
    
    QNetworkAccessManager* networkAccessManager();

    QYouTube::StreamsRequest* youtubeRequest();

    static const QString BASE_URL;
    static const QString API_URL;
    static const QString VIDEO_URL;
    static const QString RELATED_VIDEOS_URL;
    static const QString COMMENTS_URL;
    static const QString USER_URL;
    static const QString USER_VIDEOS_URL;
    static const QString STREAM_URL;

    static const QByteArray USER_AGENT;

    static const QRegExp HTML;
    static const QRegExp NEXT_PAGE;
    static const QRegExp USER_ID;
    static const QRegExp VIDEO_ID;
    
    static const int MAX_REDIRECTS;
        
    QNetworkAccessManager *m_nam;

    QYouTube::StreamsRequest *m_youtubeRequest;

    QString m_errorString;

    QVariant m_result;

    Status m_status;
    
    int m_redirects;
};

#endif // VIDEOCLIPREQUEST_H
