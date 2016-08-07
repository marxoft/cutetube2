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

#include "porntrexrequest.h"
#include "json.h"
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QStringList>

const QString PornTrexRequest::BASE_URL("http://www.porntrex.com");
const QString PornTrexRequest::SD_STREAM_URL(BASE_URL + "/mobile_src.php?id=");
const QString PornTrexRequest::HD_STREAM_URL(BASE_URL + "/mobile_hd_src.php?id=");
const QString PornTrexRequest::RELATED_VIDEOS_URL(BASE_URL + "/ajax/related_videos");

const int PornTrexRequest::MAX_REDIRECTS = 8;

PornTrexRequest::PornTrexRequest(QObject *parent) :
    ResourcesRequest(parent),
    m_nam(0),
    m_status(Null),
    m_redirects(0)
{
}

QString PornTrexRequest::errorString() const {
    return m_errorString;
}

void PornTrexRequest::setErrorString(const QString &e) {
    m_errorString = e;
}

QVariant PornTrexRequest::result() const {
    return m_result;
}

void PornTrexRequest::setResult(const QVariant &r) {
    m_result = r;
}

PornTrexRequest::Status PornTrexRequest::status() const {
    return m_status;
}

void PornTrexRequest::setStatus(PornTrexRequest::Status s) {
    if (s != status()) {
        m_status = s;
        emit statusChanged(s);
    }
}

bool PornTrexRequest::cancel() {
    if (status() == Loading) {
        setStatus(Canceled);
        emit finished();
        return true;
    }

    return false;
}

bool PornTrexRequest::get(const QString &resourceType, const QString &resourceId) {
    if ((status() == Loading) || (resourceType != "video") || (resourceId.isEmpty())) {
        return false;
    }

    getVideo(resourceId);
    return true;
}

bool PornTrexRequest::list(const QString &resourceType, const QString &resourceId) {
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

    if (resourceType == "category") {
        listCategories(resourceId);
        return true;
    }

    return false;
}

bool PornTrexRequest::search(const QString &resourceType, const QString &query, const QString &order) {
    if ((status() == Loading) || (resourceType != "video") || (query.isEmpty())) {
        return false;
    }

    searchVideos(query, order);
    return true;
}

void PornTrexRequest::getVideo(const QString &url) {
    setStatus(Loading);
    m_redirects = 0;
    QNetworkReply *reply = networkAccessManager()->get(QNetworkRequest(url));
    connect(reply, SIGNAL(finished()), this, SLOT(checkVideo()));
    connect(this, SIGNAL(finished()), reply, SLOT(deleteLater()));
}

void PornTrexRequest::checkVideo() {
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
    
    const QString result = QString::fromUtf8(reply->readAll());
    const QString description = result.section("og:description\" content=\"", 1, 1).section("\"", 0, 0);
    const QString id = reply->url().toString();
    const QString thumbnailUrl = result.section("og:image\" content=\"", 1, 1).section("\"", 0, 0);
    const QString title = result.section("og:title\" content=\"", 1, 1).section("\"", 0, 0);
    QVariantMap video;
    video["description"] = description;
    video["id"] = id;
    video["largeThumbnailUrl"] = thumbnailUrl;
    video["thumbnailUrl"] = thumbnailUrl;
    video["title"] = title;
    video["url"] = id;
    setResult(video);
    setStatus(Ready);
    emit finished();
}

void PornTrexRequest::listVideos(const QString &url) {
    if (url.contains("porntrex.com/video/")) {
        listRelatedVideos(url);
    }
    else {
        setStatus(Loading);
        m_redirects = 0;
        QNetworkReply *reply = networkAccessManager()->get(QNetworkRequest(url));
        connect(reply, SIGNAL(finished()), this, SLOT(checkVideos()));
        connect(this, SIGNAL(finished()), reply, SLOT(deleteLater()));
    }
}

void PornTrexRequest::searchVideos(const QString &query, const QString &order) {
    listVideos(QString("%1/search?search_query=%2&search_type=videos&o=%3").arg(BASE_URL).arg(query).arg(order));
}

void PornTrexRequest::checkVideos() {
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
    
    const QString result = QString::fromUtf8(reply->readAll());
    const QStringList videos = result.split("class=\"well wellov well-sm\"");
    QVariantMap response;
    QVariantList items;
    
    for (int i = 1; i < videos.size(); i++) {
        const QString &video = videos.at(i);
        const QString duration =
        video.section(video.contains("fa-clock-o") ? "class=\"fa fa-clock-o\"></i>"
                                                   : "class=\"duration\">", 1, 1).section("<", 0, 0).trimmed();
        const QString id = BASE_URL + video.section("href=\"", 1, 1).section("\"", 0, 0);
        const QString thumbnailUrl = video.section("data-original=\"", 1, 1).section("\"", 0, 0);
        const QString largeThumbnailUrl = thumbnailUrl.left(thumbnailUrl.lastIndexOf("/")) + "/default.jpg";
        const QString title = video.section("title=\"", 1, 1).section("\"", 0, 0);
        const int viewCount = qMax(0, video.section(" views", 0, 0).section(">", -1).trimmed().toInt());
        QVariantMap item;
        item["duration"] = duration;
        item["id"] = id;
        item["largeThumbnailUrl"] = largeThumbnailUrl;
        item["thumbnailUrl"] = thumbnailUrl;
        item["title"] = title;
        item["url"] = id;
        item["viewCount"] = viewCount;
        items << item;
    }
    
    response["items"] = items;
    
    if ((!items.isEmpty()) && (result.contains("\" class=\"prevnext\">Next page"))) {
        const QString next = result.section("\" class=\"prevnext\">Next page", 0, 0).section("\"", -1);
        
        if (!next.isEmpty()) {
            response["next"] = next;
        }
    }
    
    setResult(response);
    setStatus(Ready);
    emit finished();
}

void PornTrexRequest::listRelatedVideos(const QString &url) {
    setStatus(Loading);
    m_redirects = 0;
    const QString data("page=1&move=next&video_id=" + url.section("/video/", 1, 1).section("/", 0, 0));
    QNetworkRequest request(RELATED_VIDEOS_URL);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/x-www-form-urlencoded");
    request.setRawHeader("X-Requested-With", "XMLHttpRequest");
    QNetworkReply *reply = networkAccessManager()->post(request, data.toUtf8());
    connect(reply, SIGNAL(finished()), this, SLOT(checkRelatedVideos()));
    connect(this, SIGNAL(finished()), reply, SLOT(deleteLater()));
}

void PornTrexRequest::checkRelatedVideos() {
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
            followRedirect(redirect, SLOT(checkRelatedVideos()));
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
    
    const QVariantMap result = QtJson::Json::parse(QString::fromUtf8(reply->readAll())).toMap();
    const QStringList videos = result.value("videos").toString().split("class=\"well well-sm");
    QVariantMap response;
    QVariantList items;
    
    for (int i = 1; i < videos.size(); i++) {
        const QString &video = videos.at(i);
        const QString duration = video.section("class=\"duration\">", 1, 1).section("<", 0, 0).trimmed();
        const QString id = video.section("href=\"", 1, 1).section("\"", 0, 0);
        const QString thumbnailUrl = video.section("img src=\"", 1, 1).section("\"", 0, 0);
        const QString largeThumbnailUrl = thumbnailUrl.left(thumbnailUrl.lastIndexOf("/")) + "/default.jpg";
        const QString title = video.section("title=\"", 1, 1).section("\"", 0, 0);
        const int viewCount = qMax(0, video.section(" views", 0, 0).section(">", -1).toInt());
        QVariantMap item;
        item["duration"] = duration;
        item["id"] = id;
        item["largeThumbnailUrl"] = largeThumbnailUrl;
        item["thumbnailUrl"] = thumbnailUrl;
        item["title"] = title;
        item["url"] = id;
        item["viewCount"] = viewCount;
        items << item;
    }
    
    response["items"] = items;
    setResult(response);
    setStatus(Ready);
    emit finished();
}

void PornTrexRequest::listStreams(const QString &url) {
    const QString id = url.section("/video/", 1, 1).section("/", 0, 0);
    
    if (id.isEmpty()) {
        setErrorString(tr("Invalid video URL"));
        setStatus(Failed);
    }
    else {
        QVariantMap response;
        QVariantList streams;
        QVariantMap sd;
        sd["description"] = "SD";
        sd["id"] = "sd";
        sd["url"] = SD_STREAM_URL + id;
        streams << sd;
        QVariantMap hd;
        hd["description"] = "HD";
        hd["id"] = "hd";
        hd["url"] = HD_STREAM_URL + id;
        streams << hd;
        response["items"] = streams;
        setResult(response);
        setStatus(Ready);
    }
    
    emit finished();
}

void PornTrexRequest::listCategories(const QString &url) {
    setStatus(Loading);
    m_redirects = 0;
    QNetworkReply *reply = networkAccessManager()->get(QNetworkRequest(url));
    connect(reply, SIGNAL(finished()), this, SLOT(checkCategories()));
    connect(this, SIGNAL(finished()), reply, SLOT(deleteLater()));
}

void PornTrexRequest::checkCategories() {
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
            followRedirect(redirect, SLOT(checkCategories()));
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
    
    const QString result = QString::fromUtf8(reply->readAll());
    const QStringList categories = result.split("class=\"col-sm-4");
    QVariantMap response;
    QVariantList items;
    
    for (int i = 1; i < categories.size(); i++) {
        const QString &category = categories.at(i);
        const QString id = BASE_URL + category.section("href=\"", 1, 1).section("\"", 0, 0);
        const QString title = category.section("title=\"", 1, 1).section("\"", 0, 0);
        QVariantMap item;
        item["id"] = id;
        item["title"] = title;
        items << item;
    }
    
    response["items"] = items;
    setResult(response);
    setStatus(Ready);
    emit finished();
}

QString PornTrexRequest::getRedirect(const QNetworkReply *reply) {
    QString redirect = QString::fromUtf8(reply->rawHeader("Location"));
    
    if (redirect.startsWith("/")) {
        redirect.prepend(reply->url().scheme() + "://" + reply->url().authority());
    }
    
    return redirect;
}

void PornTrexRequest::followRedirect(const QString &url, const char *slot) {
    m_redirects++;
    QNetworkReply *reply = networkAccessManager()->get(QNetworkRequest(url));
    connect(reply, SIGNAL(finished()), this, slot);
    connect(this, SIGNAL(finished()), reply, SLOT(deleteLater()));
}

QNetworkAccessManager* PornTrexRequest::networkAccessManager() {
    return m_nam ? m_nam : m_nam = new QNetworkAccessManager(this);
}
