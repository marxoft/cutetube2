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

#include "wenoorequest.h"
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QRegExp>
#include <QStringList>

const QString WenooRequest::BASE_URL("http://wenoo.net");

const int WenooRequest::MAX_REDIRECTS = 8;

WenooRequest::WenooRequest(QObject *parent) :
    ResourcesRequest(parent),
    m_nam(0),
    m_status(Null),
    m_redirects(0)
{
}

QString WenooRequest::errorString() const {
    return m_errorString;
}

void WenooRequest::setErrorString(const QString &e) {
    m_errorString = e;
}

QVariant WenooRequest::result() const {
    return m_result;
}

void WenooRequest::setResult(const QVariant &r) {
    m_result = r;
}

WenooRequest::Status WenooRequest::status() const {
    return m_status;
}

void WenooRequest::setStatus(WenooRequest::Status s) {
    if (s != status()) {
        m_status = s;
        emit statusChanged(s);
    }
}

bool WenooRequest::cancel() {
    if (status() == Loading) {
        setStatus(Canceled);
        emit finished();
        return true;
    }

    return false;
}

bool WenooRequest::get(const QString &resourceType, const QString &resourceId) {
    if ((status() == Loading) || (resourceId.isEmpty())) {
        return false;
    }

    if (resourceType == "video") {
        getVideo(resourceId);
        return true;
    }

    if (resourceType == "user") {
        getUser(resourceId);
        return true;
    }

    return false;
}

bool WenooRequest::list(const QString &resourceType, const QString &resourceId) {
    if ((status() == Loading) || (resourceId.isEmpty())) {
        return false;
    }

    if (resourceType == "video") {
        listVideos(resourceId);
        return true;
    }

    if (resourceType == "user") {
        listUsers(resourceId);
        return true;
    }

    if (resourceType == "category") {
        listCategories(resourceId);
        return true;
    }

    return false;
}

bool WenooRequest::search(const QString &resourceType, const QString &query, const QString &order) {
    if ((status() == Loading) || (resourceType.isEmpty()) || (query.isEmpty())) {
        return false;
    }

    if (resourceType == "video") {
        searchVideos(query, order);
        return true;
    }

    return false;
}

void WenooRequest::getVideo(const QString &url) {
    setStatus(Loading);
    m_redirects = 0;
    QNetworkReply *reply = networkAccessManager()->get(QNetworkRequest(url));
    connect(reply, SIGNAL(finished()), this, SLOT(checkVideo()));
    connect(this, SIGNAL(finished()), reply, SLOT(deleteLater()));
}

void WenooRequest::checkVideo() {
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
    const QString streamUrl = result.section("video/mp4\" src=\"", 1, 1).section("\"", 0, 0);
    const QString thumbnailUrl = result.section("og:image\" content=\"", 1, 1).section("\"", 0, 0);
    const QString title = result.section("og:title\" content=\"", 1, 1).section("\"", 0, 0);
    const QString username = result.section("href=\"/user/", 1, 1).section("\"", 0, 0);
    const QString userId = QString("%1/user/%2").arg(BASE_URL).arg(username);
    QVariantMap video;
    video["description"] = description;
    video["id"] = id;
    video["largeThumbnailUrl"] = thumbnailUrl;
    video["relatedVideosId"] = id;
    video["streamUrl"] = streamUrl;
    video["thumbnailUrl"] = thumbnailUrl;
    video["title"] = title;
    video["url"] = id;
    video["userId"] = userId;
    video["username"] = username;
    setResult(video);
    setStatus(Ready);
    emit finished();
}

void WenooRequest::listVideos(const QString &url) {
    if (url.contains("wenoo.net/video/")) {
        listRelatedVideos(url);
    }
    else if ((url.contains("wenoo.net/user/")) || (url.contains("wenoo.net/profile/"))) {
        listUserVideos(url);
    }
    else {
        setStatus(Loading);
        m_redirects = 0;
        QNetworkReply *reply = networkAccessManager()->get(QNetworkRequest(url));
        connect(reply, SIGNAL(finished()), this, SLOT(checkVideos()));
        connect(this, SIGNAL(finished()), reply, SLOT(deleteLater()));
    }
}

void WenooRequest::searchVideos(const QString &query, const QString &order) {
    listVideos(QString("%1/search/videos/%2/%3").arg(BASE_URL).arg(query).arg(order));
}

void WenooRequest::checkVideos() {
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
    const QStringList videos = result.split("<div class=\"mbottom_10\">");
    QVariantMap response;
    QVariantList items;
    
    for (int i = 1; i < videos.size(); i++) {
        const QString &video = videos.at(i);
        const QString date = video.section("Added: </span>", 1, 1).section("&", 0, 0).trimmed();
        const QString description = video.section(video.contains("vdescr") ? "class=\"vdescr\">" : "class=\"gr\">", 1, 1)
                                                 .section("<", 0, 0).trimmed();
        const QString duration = video.section("class=\"overduration\">", 1, 1).section("<", 0, 0).trimmed();
        const QString id = video.section("href=\"", 2, 2).section("\"", 0, 0);
        const QString streamId = video.section("id=\"l", 3, 3).section("\"", 0, 0);
        const QString thumbnailUrl = video.section("src=\"", 1, 1).section("\"", 0, 0);
        const QString largeThumbnailUrl = QString("%1/pl_%2.jpg").arg(thumbnailUrl.left(thumbnailUrl.lastIndexOf("/")))
                                                                 .arg(streamId);
        const QString streamUrl = QString("%1/%2.mp4").arg(thumbnailUrl.left(thumbnailUrl.lastIndexOf("/")))
                                                      .arg(streamId).replace("thumbnail", "flv");
        const QString title = video.section("title=\"", 2, 2).section("\"", 0, 0);
        const QString userId = video.section("class=\"user\" href=\"", 1, 1).section("\"", 0, 0);
        const QString username = userId.mid(userId.lastIndexOf("/") + 1);
        const int viewCount = qMax(0, video.section("Views: </span>", 1, 1).section(" ", 0, 0).toInt());
        QVariantMap item;
        item["date"] = date;
        item["description"] = description;
        item["duration"] = duration;
        item["id"] = id;
        item["largeThumbnailUrl"] = largeThumbnailUrl;
        item["relatedVideosId"] = id;
        item["streamUrl"] = streamUrl;
        item["thumbnailUrl"] = thumbnailUrl;
        item["title"] = title;
        item["url"] = id;
        item["userId"] = userId;
        item["username"] = username;
        item["viewCount"] = viewCount;
        items << item;
    }
    
    response["items"] = items;
    
    if ((!items.isEmpty()) && (result.contains("'><span class=\"pag_prev\">NEXT"))) {
        const QString next = result.section("'><span class=\"pag_prev\">NEXT", 0, 0).section("'", -1);
        
        if (!next.isEmpty()) {
            response["next"] = next;
        }
    }
    
    setResult(response);
    setStatus(Ready);
    emit finished();
}

void WenooRequest::listRelatedVideos(const QString &url) {
    setStatus(Loading);
    m_redirects = 0;
    QNetworkReply *reply = networkAccessManager()->get(QNetworkRequest(url));
    connect(reply, SIGNAL(finished()), this, SLOT(checkRelatedVideos()));
    connect(this, SIGNAL(finished()), reply, SLOT(deleteLater()));
}

void WenooRequest::checkRelatedVideos() {
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
    
    const QString result = QString::fromUtf8(reply->readAll());
    const QStringList videos = result.split("<div class=\"sbVid\">");
    QVariantMap response;
    QVariantList items;
    
    for (int i = 1; i < videos.size(); i++) {
        const QString &video = videos.at(i);
        const QString duration = video.section("<b>", 1, 1).section("<", 0, 0).trimmed();
        const QString id = BASE_URL + video.section("href=\"", 2, 2).section("\"", 0, 0);
        const QString thumbnailUrl = BASE_URL + video.section("src=\"", 1, 1).section("\"", 0, 0);
        const QString streamId = thumbnailUrl.section("_", -1).section(".", 0, 0);
        const QString largeThumbnailUrl = QString("%1/pl_%2.jpg").arg(thumbnailUrl.left(thumbnailUrl.lastIndexOf("/")))
                                                                 .arg(streamId);
        const QString streamUrl = QString("%1/%2.mp4").arg(thumbnailUrl.left(thumbnailUrl.lastIndexOf("/")))
                                                      .arg(streamId).replace("thumbnail", "flv");
        const QString title = video.section("title=\"", 1, 1).section("\"", 0, 0);
        const QString userId = BASE_URL + video.section("href=\"", 3, 3).section("\"", 0, 0);
        const QString username = userId.mid(userId.lastIndexOf("/") + 1);
        const int viewCount = qMax(0, video.section(" views", 0, 0).section(">", -1).toInt());
        QVariantMap item;
        item["duration"] = duration;
        item["id"] = id;
        item["largeThumbnailUrl"] = largeThumbnailUrl;
        item["relatedVideosId"] = id;
        item["streamUrl"] = streamUrl;
        item["thumbnailUrl"] = thumbnailUrl;
        item["title"] = title;
        item["url"] = id;
        item["userId"] = userId;
        item["username"] = username;
        item["viewCount"] = viewCount;
        items << item;
    }
    
    response["items"] = items;
    setResult(response);
    setStatus(Ready);
    emit finished();
}

void WenooRequest::listUserVideos(const QString &url) {
    setStatus(Loading);
    m_redirects = 0;
    QNetworkReply *reply =
    networkAccessManager()->get(QNetworkRequest(QString(url + "&view=videos").replace("/profile/", "/user/")));
    connect(reply, SIGNAL(finished()), this, SLOT(checkUserVideos()));
    connect(this, SIGNAL(finished()), reply, SLOT(deleteLater()));
}

void WenooRequest::checkUserVideos() {
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
            followRedirect(redirect, SLOT(checkUserVideos()));
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
    const QStringList videos = result.split("<div class=\"left mbottom_10");
    QVariantMap response;
    QVariantList items;
    
    for (int i = 1; i < videos.size(); i++) {
        const QString &video = videos.at(i);
        const QString date = video.section("class=\"ch_highlight\">", 1, 1).section("<", 0, 0);
        const QString duration = video.section("class=\"overduration\">", 1, 1).section("<", 0, 0).trimmed();
        const QString id = video.section("href=\"", 2, 2).section("\"", 0, 0);
        const QString streamId = video.section("<img id=\"", 1, 1).section("\"", 0, 0);
        const QString thumbnailUrl = video.section("src=\"", 1, 1).section("\"", 0, 0);
        const QString largeThumbnailUrl = QString("%1/pl_%2.jpg").arg(thumbnailUrl.left(thumbnailUrl.lastIndexOf("/")))
                                                                 .arg(streamId);
        const QString streamUrl = QString("%1/%2.mp4").arg(thumbnailUrl.left(thumbnailUrl.lastIndexOf("/")))
                                                      .arg(streamId).replace("thumbnail", "flv");
        const QString title = video.section("title=\"", 2, 2).section("\"", 0, 0);
        const QString userId = reply->url().toString().section("&", 0, -2);
        const QString username = userId.mid(userId.lastIndexOf("/") + 1);
        const int viewCount = qMax(0, video.section("class=\"ch_highlight\">", 2, 2).section("<", 0, 0).toInt());
        QVariantMap item;
        item["date"] = date;
        item["duration"] = duration;
        item["id"] = id;
        item["largeThumbnailUrl"] = largeThumbnailUrl;
        item["relatedVideosId"] = id;
        item["streamUrl"] = streamUrl;
        item["thumbnailUrl"] = thumbnailUrl;
        item["title"] = title;
        item["url"] = id;
        item["userId"] = userId;
        item["username"] = username;
        item["viewCount"] = viewCount;
        items << item;
    }
    
    response["items"] = items;
    
    if ((!items.isEmpty()) && (result.contains("'><span class=\"pag_prev\">NEXT"))) {
        const QString next = result.section("'><span class=\"pag_prev\">NEXT", 0, 0).section("'", -1);
        
        if (!next.isEmpty()) {
            response["next"] = next;
        }
    }
    
    setResult(response);
    setStatus(Ready);
    emit finished();
}

void WenooRequest::getUser(const QString &url) {
    setStatus(Loading);
    m_redirects = 0;
    QNetworkReply *reply = networkAccessManager()->get(QNetworkRequest(url));
    connect(reply, SIGNAL(finished()), this, SLOT(checkUser()));
    connect(this, SIGNAL(finished()), reply, SLOT(deleteLater()));
}

void WenooRequest::checkUser() {
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
            followRedirect(redirect, SLOT(checkUser()));
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
    const QString username = id.mid(id.lastIndexOf("/") + 1);
    const QString thumbnailUrl = QString("%1/media/files_user_avatar/%2").arg(BASE_URL)
                                        .arg(result.section("/media/files_user_avatar/", 1, 1).section("\"", 0, 0));
    QVariantMap user;
    user["description"] = description;
    user["id"] = id;
    user["largeThumbnailUrl"] = thumbnailUrl;
    user["thumbnailUrl"] = thumbnailUrl;
    user["username"] = username;
    user["videosId"] = id;
    setResult(user);
    setStatus(Ready);
    emit finished();
}

void WenooRequest::listUsers(const QString &url) {
    setStatus(Loading);
    m_redirects = 0;
    QNetworkReply *reply = networkAccessManager()->get(QNetworkRequest(url));
    connect(reply, SIGNAL(finished()), this, SLOT(checkUsers()));
    connect(this, SIGNAL(finished()), reply, SLOT(deleteLater()));
}

void WenooRequest::checkUsers() {
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
            followRedirect(redirect, SLOT(checkUsers()));
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
    const QStringList users = result.split("<div class=\"list_channel left\">");
    QVariantMap response;
    QVariantList items;
    
    for (int i = 1; i < users.size(); i++) {
        const QString &user = users.at(i);
        const QString id = user.section("href=\"", 1, 1).section("\"", 0, 0);
        const QString thumbnailUrl = user.section("src=\"", 1, 1).section("\"", 0, 0);
        const QString username = id.mid(id.lastIndexOf("/") + 1);
        QVariantMap item;
        item["id"] = id;
        item["largeThumbnailUrl"] = thumbnailUrl;
        item["thumbnailUrl"] = thumbnailUrl;
        item["username"] = username;
        item["videosId"] = id;
        items << item;
    }
    
    response["items"] = items;
    
    if ((!items.isEmpty()) && (result.contains("'><span class=\"pag_prev\">NEXT"))) {
        const QString next = result.section("'><span class=\"pag_prev\">NEXT", 0, 0).section("'", -1);
        
        if (!next.isEmpty()) {
            response["next"] = next;
        }
    }
    
    setResult(response);
    setStatus(Ready);
    emit finished();
}

void WenooRequest::listCategories(const QString &url) {
    setStatus(Loading);
    m_redirects = 0;
    QNetworkReply *reply = networkAccessManager()->get(QNetworkRequest(url));
    connect(reply, SIGNAL(finished()), this, SLOT(checkCategories()));
    connect(this, SIGNAL(finished()), reply, SLOT(deleteLater()));
}

void WenooRequest::checkCategories() {
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
    const QStringList categories = result.split("<div class=\"left cat-main\"");
    QVariantMap response;
    QVariantList items;
    
    for (int i = 1; i < categories.size(); i++) {
        const QString &category = categories.at(i);
        const QString id = category.section("class=\"cat-small-a\" href=\"", 1, 1).section("\"", 0, 0);
        const QString title = category.section("alt=\"", 1, 1).section("\"", 0, 0);
        QVariantMap item;
        item["id"] = id;
        item["title"] = title;
        item["videosId"] = id;
        items << item;
    }
    
    response["items"] = items;
    
    if ((!items.isEmpty()) && (result.contains("'><span class=\"pag_prev\">NEXT"))) {
        const QString next = result.section("'><span class=\"pag_prev\">NEXT", 0, 0).section("'", -1);
        
        if (!next.isEmpty()) {
            response["next"] = next;
        }
    }
    
    setResult(response);
    setStatus(Ready);
    emit finished();
}

QString WenooRequest::getRedirect(const QNetworkReply *reply) {
    QString redirect = QString::fromUtf8(reply->rawHeader("Location"));
    
    if (redirect.startsWith("/")) {
        redirect.prepend(reply->url().scheme() + "://" + reply->url().authority());
    }
    
    return redirect;
}

void WenooRequest::followRedirect(const QString &url, const char *slot) {
    m_redirects++;
    QNetworkReply *reply = networkAccessManager()->get(QNetworkRequest(url));
    connect(reply, SIGNAL(finished()), this, slot);
    connect(this, SIGNAL(finished()), reply, SLOT(deleteLater()));
}

QNetworkAccessManager* WenooRequest::networkAccessManager() {
    return m_nam ? m_nam : m_nam = new QNetworkAccessManager(this);
}
