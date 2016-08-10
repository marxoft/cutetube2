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

#include "veohrequest.h"
#include "json.h"
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#if QT_VERSION >= 0x050000
#include <QUrlQuery>
#endif

const QString VeohRequest::BASE_URL("http://www.veoh.com");
const QString VeohRequest::SEARCH_URL(BASE_URL + "/find/");
const QString VeohRequest::VIDEO_URL(BASE_URL + "/watch/");

const QByteArray VeohRequest::USER_AGENT("Wget/1.13.4 (linux-gnu)");

const QRegExp VeohRequest::VIDEO_ENTRY("id=\"json_browse_\\d+\">([^<]+)");

const int VeohRequest::MAX_REDIRECTS = 8;

VeohRequest::VeohRequest(QObject *parent) :
    ResourcesRequest(parent),
    m_nam(0),
    m_status(Null),
    m_redirects(0)
{
}

QString VeohRequest::errorString() const {
    return m_errorString;
}

void VeohRequest::setErrorString(const QString &e) {
    m_errorString = e;
}

QVariant VeohRequest::result() const {
    return m_result;
}

void VeohRequest::setResult(const QVariant &r) {
    m_result = r;
}

VeohRequest::Status VeohRequest::status() const {
    return m_status;
}

void VeohRequest::setStatus(VeohRequest::Status s) {
    if (s != status()) {
        m_status = s;
        emit statusChanged(s);
    }
}

bool VeohRequest::cancel() {
    if (status() == Loading) {
        setStatus(Canceled);
        emit finished();
        return true;
    }

    return false;
}

bool VeohRequest::get(const QString &resourceType, const QString &resourceId) {
    if ((status() == Loading) || (resourceId.isEmpty())) {
        return false;
    }

    if (resourceType == "video") {
        getVideo(resourceId);
        return true;
    }

    return false;
}

bool VeohRequest::list(const QString &resourceType, const QString &resourceId) {
    if ((status() == Loading) || (resourceId.isEmpty())) {
        return false;
    }

    if (resourceType == "video") {
        listVideos(resourceId);
        return true;
    }
    
    if (resourceType == "stream") {
        listStreams(resourceId);
        return true;
    }

    return false;
}

bool VeohRequest::search(const QString &resourceType, const QString &query, const QString &order) {
    if ((status() == Loading) || (resourceType.isEmpty()) || (query.isEmpty())) {
        return false;
    }

    if (resourceType == "video") {
        searchVideos(query, order);
        return true;
    }

    return false;
}

void VeohRequest::getVideo(const QString &url) {
    setStatus(Loading);
    m_redirects = 0;
    QNetworkRequest request(url);
    request.setRawHeader("User-Agent", USER_AGENT);
    QNetworkReply *reply = networkAccessManager()->get(request);
    connect(reply, SIGNAL(finished()), this, SLOT(checkVideo()));
    connect(this, SIGNAL(finished()), reply, SLOT(deleteLater()));
}

void VeohRequest::checkVideo() {
    QNetworkReply *reply = qobject_cast<QNetworkReply*>(sender());
    
    if (!reply) {
        setErrorString(tr("Network error"));
        setStatus(Failed);
        emit finished();
        return;
    }
    
    const QString redirect = getRedirect(reply);
    
    if (!redirect.isEmpty()) {
        if (m_redirects < MAX_REDIRECTS) {
            followRedirect(redirect, SLOT(checkVideo()));
        }
        else {
            setErrorString(tr("Maximum redirects reached"));
            setStatus(Failed);
            emit finished();
        }
        
        return;
    }
    
    if (reply->error() != QNetworkReply::NoError) {
        setErrorString(reply->errorString());
        setStatus(Failed);
        emit finished();
        return;
    }
    
    const QString response = QString::fromUtf8(reply->readAll());
    const QVariantMap json = QtJson::Json::parse(response.section("videoDetailsJSON = '", 1, 1)
                                                         .section("';", 0, 0)).toMap();
    
    if (json.isEmpty()) {
        setErrorString(tr("No video info found"));
        setResult(QVariant());
        setStatus(Failed);
    }
    else {
        QVariantMap video;
        const QVariant id = json.value("videoPageUrl");
        video["date"] = json.value("age");
        video["description"] = json.value("description");
        video["duration"] = json.value("length").toInt();
        video["id"] = id;
        video["largeThumbnailUrl"] = json.value("highResImage");
        video["relatedVideosId"] = id;
        video["thumbnailUrl"] = json.value("medResImage");
        video["title"] = json.value("title");
        video["url"] = id;
        video["viewCount"] = json.value("views").toInt();
        setErrorString(QString());
        setResult(video);
        setStatus(Ready);
    }
    
    emit finished();
}

void VeohRequest::listVideos(const QString &url) {
    setStatus(Loading);
    m_redirects = 0;
    QNetworkRequest request(url);
    request.setRawHeader("User-Agent", USER_AGENT);
    QNetworkReply *reply = networkAccessManager()->get(request);
    connect(reply, SIGNAL(finished()), this, SLOT(checkVideos()));
    connect(this, SIGNAL(finished()), reply, SLOT(deleteLater()));
}

void VeohRequest::searchVideos(const QString &query, const QString &order) {
    listVideos(QString("%1?mode=video&cat=videos&query=%2&sort=%3").arg(SEARCH_URL).arg(query).arg(order));
}

void VeohRequest::checkVideos() {
    QNetworkReply *reply = qobject_cast<QNetworkReply*>(sender());
    
    if (!reply) {
        setErrorString(tr("Network error"));
        setStatus(Failed);
        emit finished();
        return;
    }
    
    const QString redirect = getRedirect(reply);
    
    if (!redirect.isEmpty()) {
        if (m_redirects < MAX_REDIRECTS) {
            followRedirect(redirect, SLOT(checkVideos()));
        }
        else {
            setErrorString(tr("Maximum redirects reached"));
            setStatus(Failed);
            emit finished();
        }
        
        return;
    }
    
    if (reply->error() != QNetworkReply::NoError) {
        setErrorString(reply->errorString());
        setStatus(Failed);
        emit finished();
        return;
    }
    
    const QString response = QString::fromUtf8(reply->readAll());
    QVariantMap result;
    QVariantList videos;

    if (response.contains("relatedVideosJSON")) {
        const QVariantList related = QtJson::Json::parse(response.section("relatedVideosJSON = '", 1, 1)
                                                                 .section("';", 0, 0)).toMap().value("items").toList();

        foreach (const QVariant &r, related) {
            const QVariantMap rv = r.toMap();
            const QString id = VIDEO_URL + rv.value("permalinkId").toString();
            QVariantMap video;
            video["duration"] = rv.value("length").toInt();
            video["id"] = id;
            video["largeThumbnailUrl"] = rv.value("highResImage");
            video["relatedVideosId"] = id;
            video["thumbnailUrl"] = rv.value("medResImage");
            video["title"] = rv.value("title");
            video["url"] = video.value("id");
            videos << video;
        }
    }
    else {
        int pos = 0;
        int results = 0;
        
        while ((pos = VIDEO_ENTRY.indexIn(response, pos)) != -1) {
            const QVariantMap json = QtJson::Json::parse(VIDEO_ENTRY.cap(1).trimmed()).toMap();
            
            if ((!json.isEmpty()) && (json.value("isExternalMedia") == "0")) {
                QVariantMap video;
                const QString id = VIDEO_URL + json.value("videoId").toString();
                video["duration"] = json.value("length").toInt();
                video["id"] = id;
                video["largeThumbnailUrl"] = json.value("thumbnailUrl");
                video["relatedVideosId"] = id;
                video["thumbnailUrl"] = json.value("thumbnailUrl");
                video["title"] = json.value("title");
                video["url"] = id;
                videos << video;
            }
            
            pos += VIDEO_ENTRY.matchedLength();
            ++results;
        }
        
        if (results > 0) {
            QUrl url = reply->url();
#if QT_VERSION >= 0x050000
            QUrlQuery query(url);
            int offset = query.queryItemValue("offset").toInt();
            
            if (offset == -1) {
                offset = results;
            }
            else {
                offset += results;
            }
            
            query.removeQueryItem("offset");
            query.addQueryItem("offset", QString::number(offset));
            url.setQuery(query);
#else
            int offset = url.queryItemValue("offset").toInt();
            
            if (offset == -1) {
                offset = results;
            }
            else {
                offset += results;
            }
            
            url.removeQueryItem("offset");
            url.addQueryItem("offset", QString::number(offset));
#endif
            result["next"] = url;
        }        
    }

    result["items"] = videos;
    setResult(result);
    setErrorString(QString());
    setStatus(Ready);
    emit finished();
}

void VeohRequest::listStreams(const QString &url) {
    setStatus(Loading);
    m_redirects = 0;
    QNetworkRequest request(url);
    request.setRawHeader("User-Agent", USER_AGENT);
    QNetworkReply *reply = networkAccessManager()->get(request);
    connect(reply, SIGNAL(finished()), this, SLOT(checkStreams()));
    connect(this, SIGNAL(finished()), reply, SLOT(deleteLater()));
}

void VeohRequest::checkStreams() {
    QNetworkReply *reply = qobject_cast<QNetworkReply*>(sender());
    
    if (!reply) {
        setErrorString(tr("Network error"));
        setStatus(Failed);
        emit finished();
        return;
    }
    
    QString redirect = getRedirect(reply);
    
    if (!redirect.isEmpty()) {
        if (m_redirects < MAX_REDIRECTS) {
            followRedirect(redirect, SLOT(checkStreams()));
        }
        else {
            setErrorString(tr("Maximum redirects reached"));
            setStatus(Failed);
            emit finished();
        }
        
        return;
    }
    
    if (reply->error() != QNetworkReply::NoError) {
        setErrorString(reply->errorString());
        setStatus(Failed);
        emit finished();
        return;
    }
    
    const QString response = QString::fromUtf8(reply->readAll());
    const QVariantMap json = QtJson::Json::parse(response.section("videoDetailsJSON = '", 1, 1)
                                                         .section("';", 0, 0)).toMap();
    
    if (json.isEmpty()) {
        setErrorString(tr("No streams found"));
        setResult(QVariant());
        setStatus(Failed);
    }
    else {
        QVariantMap result;
        QVariantList streams;
        
        if (json.contains("fullPreviewHashHighPath")) {
            QVariantMap hq;
            hq["description"] = "HQ";
            hq["ext"] = "mp4";
            hq["id"] = "hq";
            hq["url"] = json.value("fullPreviewHashHighPath");
            streams << hq;
        }

        if (json.contains("fullPreviewHashLowPath")) {
            QVariantMap lq;
            lq["description"] = "LQ";
            lq["ext"] = "mp4";
            lq["id"] = "hq";
            lq["url"] = json.value("fullPreviewHashLowPath");
            streams << lq;
        }

        result["items"] = streams;
        setErrorString(QString());
        setResult(result);
        setStatus(Ready);
    }

    emit finished();
}

QString VeohRequest::getRedirect(const QNetworkReply *reply) {
    QString redirect = QString::fromUtf8(reply->rawHeader("Location"));
    
    if (redirect.startsWith("/")) {
        redirect.prepend(reply->url().scheme() + "://" + reply->url().authority());
    }
    
    return redirect;
}

void VeohRequest::followRedirect(const QString &url, const char *slot) {
    m_redirects++;
    QNetworkRequest request(url);
    request.setRawHeader("User-Agent", USER_AGENT);
    QNetworkReply *reply = networkAccessManager()->get(request);
    connect(reply, SIGNAL(finished()), this, slot);
    connect(this, SIGNAL(finished()), reply, SLOT(deleteLater()));
}

QNetworkAccessManager* VeohRequest::networkAccessManager() {
    return m_nam ? m_nam : m_nam = new QNetworkAccessManager(this);
}
