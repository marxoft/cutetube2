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

#include "videocliprequest.h"
#include "json.h"
#include <qyoutube/streamsrequest.h>
#include <QDateTime>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QStringList>

const QString VideoclipRequest::BASE_URL("http://www.videoclip.bg");
const QString VideoclipRequest::API_URL(BASE_URL + "/api");
const QString VideoclipRequest::VIDEO_URL(API_URL + "/video/%1");
const QString VideoclipRequest::RELATED_VIDEOS_URL(VIDEO_URL + "/related");
const QString VideoclipRequest::COMMENTS_URL(VIDEO_URL + "/comments");
const QString VideoclipRequest::USER_URL(API_URL + "/user/%1");
const QString VideoclipRequest::USER_VIDEOS_URL(BASE_URL + "/fragment/user/%1/videos");
const QString VideoclipRequest::STREAM_URL(BASE_URL + "/watch/%1");

const QByteArray VideoclipRequest::USER_AGENT("Wget/1.13.4 (linux-gnu)");

const QRegExp VideoclipRequest::HTML("<[^>]*>");
const QRegExp VideoclipRequest::NEXT_PAGE(QString::fromUtf8("data-page=\"\\d+\"\\s+href=\"([^\"]+)\"\\s+>\\s+Следваща"));
const QRegExp VideoclipRequest::USER_ID("(^[\\w-_]+$|/user/[\\w-_]+)");
const QRegExp VideoclipRequest::VIDEO_ID("^\\d+$");

const int VideoclipRequest::MAX_REDIRECTS = 8;

VideoclipRequest::VideoclipRequest(QObject *parent) :
    ResourcesRequest(parent),
    m_nam(0),
    m_youtubeRequest(0),
    m_status(Null),
    m_redirects(0)
{
}

QString VideoclipRequest::errorString() const {
    return m_errorString;
}

void VideoclipRequest::setErrorString(const QString &e) {
    m_errorString = e;
}

QVariant VideoclipRequest::result() const {
    return m_result;
}

void VideoclipRequest::setResult(const QVariant &r) {
    m_result = r;
}

VideoclipRequest::Status VideoclipRequest::status() const {
    return m_status;
}

void VideoclipRequest::setStatus(VideoclipRequest::Status s) {
    if (s != status()) {
        m_status = s;
        emit statusChanged(s);
    }
}

bool VideoclipRequest::cancel() {
    if (status() == Loading) {
        setStatus(Canceled);
        emit finished();
        return true;
    }

    return false;
}

bool VideoclipRequest::get(const QString &resourceType, const QString &resourceId) {
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

bool VideoclipRequest::list(const QString &resourceType, const QString &resourceId) {
    if ((status() == Loading) || (resourceId.isEmpty())) {
        return false;
    }

    if (resourceType == "video") {
        listVideos(resourceId);
        return true;
    }

    if (resourceType == "comment") {
        listComments(resourceId);
        return true;
    }

    if (resourceType == "user") {
        listUsers(resourceId);
        return true;
    }

    if (resourceType == "stream") {
        listStreams(resourceId);
        return true;
    }

    return false;
}

bool VideoclipRequest::search(const QString &resourceType, const QString &query, const QString &order) {
    if ((status() == Loading) || (resourceType.isEmpty()) || (query.isEmpty())) {
        return false;
    }

    if (resourceType == "video") {
        searchVideos(query, order);
        return true;
    }

    return false;
}

void VideoclipRequest::getVideo(const QString &id) {
    setStatus(Loading);
    m_redirects = 0;
    QNetworkRequest request;

    if (id.contains("/watch/")) {
        request.setUrl(VIDEO_URL.arg(id.section("/watch/", -1).section("_", 0, 0)));
    }
    else {
        request.setUrl(VIDEO_URL.arg(id));
    }
    
    request.setRawHeader("User-Agent", USER_AGENT);
    QNetworkReply *reply = networkAccessManager()->get(request);
    connect(reply, SIGNAL(finished()), this, SLOT(checkVideo()));
    connect(this, SIGNAL(finished()), reply, SLOT(deleteLater()));
}

void VideoclipRequest::checkVideo() {
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
    const QVariantMap video = QtJson::Json::parse(response).toMap();

    if (video.isEmpty()) {
        setErrorString(tr("No video found"));
        setResult(QVariant());
        setStatus(Failed);
    }
    else {
        QVariantMap item;
        item["date"] = video.value("date").toDateTime().toString("dd MMM yyyy");
        item["duration"] = video.value("duration", 0);
        item["id"] = video.value("id");
        item["largeThumbnailUrl"] = video.value("t_360");
        item["thumbnailUrl"] = video.value("t_96");
        item["title"] = video.value("title");
        item["url"] = video.value("url");
        item["userId"] = video.value("uploader");
        item["username"] = video.value("uploader");
        item["viewCount"] = video.value("views", 0);
        setErrorString(QString());
        setResult(item);
        setStatus(Ready);
    }

    emit finished();
}

void VideoclipRequest::listVideos(const QString &url) {
    if (url.contains(VIDEO_ID)) {
        listRelatedVideos(url);
        return;
    }

    if ((url.contains(USER_ID))) {
        listUserVideos(url);
        return;
    }
    
    setStatus(Loading);
    m_redirects = 0;
    QNetworkRequest request(url);
    request.setRawHeader("User-Agent", USER_AGENT);
    QNetworkReply *reply = networkAccessManager()->get(request);
    connect(reply, SIGNAL(finished()), this, SLOT(checkVideos()));
    connect(this, SIGNAL(finished()), reply, SLOT(deleteLater()));
}

void VideoclipRequest::searchVideos(const QString &query, const QString &order) {
    listVideos(QString("%1/search?search_query=%2&sort=%3").arg(BASE_URL).arg(query).arg(order));
}

void VideoclipRequest::checkVideos() {
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
    const QStringList videos = response.split("class=\"item\"");
    QVariantMap result;
    QVariantList items;
    
    for (int i = 1; i < videos.size(); i++) {
        const QString &video = videos.at(i);
        const QString date = video.section("data-date=\"", 1, 1).section("\"", 0, 0);
        const QString duration = video.section("<span class=\"thumb-duration\">", 1, 1).section("<", 0, 0);
        const QString id = video.section("data-video=\"", 1, 1).section("\"", 0, 0);
        const QString largeThumbnailUrl = "http:" + video.section("src=\"", 1, 1).section("\"", 0, 0);
        const QString thumbnailUrl = largeThumbnailUrl.left(largeThumbnailUrl.lastIndexOf("_") + 1) + "96.jpeg";
        const QString title = video.section("title=\"", 1, 1).section("\"", 0, 0);
        const QString url = BASE_URL + video.section("href=\"", 1, 1).section("\"", 0, 0);
        const QString userId = BASE_URL + video.section("href=\"", -1).section("\"", 0, 0);
        const QString username = video.section("data-username=\"", 1, 1).section("\"", 0, 0);
        QVariantMap item;
        item["duration"] = duration;
        item["id"] = id;
        item["largeThumbnailUrl"] = largeThumbnailUrl;
        item["thumbnailUrl"] = thumbnailUrl;
        item["title"] = title;
        item["url"] = url;
        item["userId"] = userId;
        item["username"] = username;
        
        if (!date.isEmpty()) {
            item["date"] = QDateTime::fromString(date, Qt::ISODate).toString("dd MMM yyyy");
        }
        
        items << item;
    }
    
    result["items"] = items;
    
    if ((!items.isEmpty()) && (NEXT_PAGE.indexIn(response) != -1)) {
        result["next"] = BASE_URL + NEXT_PAGE.cap(1);
    }
    
    setResult(result);
    setStatus(Ready);
    emit finished();
}

void VideoclipRequest::listRelatedVideos(const QString &id) {
    setStatus(Loading);
    m_redirects = 0;
    QNetworkRequest request(RELATED_VIDEOS_URL.arg(id));
    request.setRawHeader("User-Agent", USER_AGENT);
    QNetworkReply *reply = networkAccessManager()->get(request);
    connect(reply, SIGNAL(finished()), this, SLOT(checkRelatedVideos()));
    connect(this, SIGNAL(finished()), reply, SLOT(deleteLater()));
}

void VideoclipRequest::checkRelatedVideos() {
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
    
    const QString response = QString::fromUtf8(reply->readAll());
    const QVariantList videos = QtJson::Json::parse(response).toList();
    QVariantMap result;
    QVariantList items;

    foreach (const QVariant &v, videos) {
        const QVariantMap video = v.toMap();
        QVariantMap item;
        item["date"] = video.value("date").toDateTime().toString("dd MMM yyyy");
        item["duration"] = video.value("duration", 0);
        item["id"] = video.value("id");
        item["largeThumbnailUrl"] = video.value("t_360");
        item["thumbnailUrl"] = video.value("t_96");
        item["title"] = video.value("title");
        item["url"] = video.value("url");
        item["userId"] = video.value("uploader");
        item["username"] = video.value("uploader");
        item["viewCount"] = video.value("views", 0);
        items << item;
    }

    result["items"] = items;
    setErrorString(QString());
    setResult(result);
    setStatus(Ready);
    emit finished();
}

void VideoclipRequest::listUserVideos(const QString &id) {
    setStatus(Loading);
    m_redirects = 0;
    QNetworkRequest request;

    if (!id.contains("/user/")) {
        request.setUrl(USER_VIDEOS_URL.arg(id));
    }
    else {
        request.setUrl(id);
    }
    
    request.setRawHeader("User-Agent", USER_AGENT);
    QNetworkReply *reply = networkAccessManager()->get(request);
    connect(reply, SIGNAL(finished()), this, SLOT(checkUserVideos()));
    connect(this, SIGNAL(finished()), reply, SLOT(deleteLater()));
}

void VideoclipRequest::checkUserVideos() {
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
    
    const QString response = QString::fromUtf8(reply->readAll());
    const QString username = reply->url().toString().section("/user/", -1).section("/", 0, 0);
    const QStringList videos = response.split("class=\"item\"");
    QVariantMap result;
    QVariantList items;
    
    for (int i = 1; i < videos.size(); i++) {
        const QString &video = videos.at(i);
        const QString date = QDateTime::fromString(video.section("data-date=\"", 1, 1).section("\"", 0, 0),
                                                   Qt::ISODate).toString("dd MMM yyyy");
        const QString duration = video.section("<span class=\"thumb-duration\">", 1, 1).section("<", 0, 0);
        const QString id = video.section("data-video=\"", 1, 1).section("\"", 0, 0);
        const QString largeThumbnailUrl = "http:" + video.section("src=\"", 1, 1).section("\"", 0, 0);
        const QString thumbnailUrl = largeThumbnailUrl.left(largeThumbnailUrl.lastIndexOf("_") + 1) + "96.jpeg";
        const QString title = video.section("title=\"", 1, 1).section("\"", 0, 0);
        const QString url = BASE_URL + video.section("href=\"", 1, 1).section("\"", 0, 0);
        const QString userId = QString("%1/user/%2").arg(BASE_URL).arg(username);
        QVariantMap item;
        item["date"] = date;
        item["duration"] = duration;
        item["id"] = id;
        item["largeThumbnailUrl"] = largeThumbnailUrl;
        item["thumbnailUrl"] = thumbnailUrl;
        item["title"] = title;
        item["url"] = url;
        item["userId"] = userId;
        item["username"] = username;
        items << item;
    }
    
    result["items"] = items;
    
    if (!items.isEmpty()) {
        QString next = reply->url().toString();

        if (next.contains("/page/")) {
            const int sep = next.lastIndexOf("/");
            next = next.left(sep + 1) + QString::number(next.mid(sep + 1).toInt() + 1);
        }
        else {
            if (!next.endsWith("/")) {
                next.append("/");
            }
            
            next.append("page/2");
        }

        result["next"] = next;
    }
    
    setResult(result);
    setStatus(Ready);
    emit finished();
}

void VideoclipRequest::listComments(const QString &id) {
    setStatus(Loading);
    m_redirects = 0;
    QNetworkRequest request(COMMENTS_URL.arg(id));
    request.setRawHeader("User-Agent", USER_AGENT);
    QNetworkReply *reply = networkAccessManager()->get(request);
    connect(reply, SIGNAL(finished()), this, SLOT(checkComments()));
    connect(this, SIGNAL(finished()), reply, SLOT(deleteLater()));
}

void VideoclipRequest::checkComments() {
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
            followRedirect(redirect, SLOT(checkComments()));
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
    const QVariantList comments = QtJson::Json::parse(response).toList();
    QVariantMap result;
    QVariantList items;

    foreach (const QVariant &c, comments) {
        const QVariantMap comment = c.toMap();
        QVariantMap item;
        item["body"] = comment.value("text").toString().remove(HTML).trimmed();
        item["date"] = comment.value("date").toDateTime().toString("dd MMM yyyy");
        item["id"] = comment.value("id");
        item["thumbnailUrl"] = "http:" + comment.value("user_avatar").toString();
        item["userId"] = comment.value("user");
        item["username"] = comment.value("user");
        items << item;
    }

    result["items"] = items;
    setErrorString(QString());
    setResult(result);
    setStatus(Ready);
    emit finished();
}

void VideoclipRequest::getUser(const QString &id) {
    setStatus(Loading);
    m_redirects = 0;
    QNetworkRequest request;

    if (id.contains("/user/")) {
        request.setUrl(USER_URL.arg(id.section("/user/", -1).section("/", 0, 0)));
    }
    else {
        request.setUrl(USER_URL.arg(id));
    }
    
    request.setRawHeader("User-Agent", USER_AGENT);
    QNetworkReply *reply = networkAccessManager()->get(request);
    connect(reply, SIGNAL(finished()), this, SLOT(checkUser()));
    connect(this, SIGNAL(finished()), reply, SLOT(deleteLater()));
}

void VideoclipRequest::checkUser() {
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
    
    const QString response = QString::fromUtf8(reply->readAll());
    const QVariantMap user = QtJson::Json::parse(response).toMap();

    if (user.isEmpty()) {
        setErrorString(tr("No user found"));
        setResult(QVariant());
        setStatus(Failed);
    }
    else {
        const QVariant username = user.value("username");
        const QString avatar = "http:" + user.value("avatar").toString();
        QVariantMap item;
        item["id"] = username;
        item["largeThumbnailUrl"] = avatar;
        item["thumbnailUrl"] = avatar;
        item["username"] = username;
        setErrorString(QString());
        setResult(item);
        setStatus(Ready);
    }
    
    emit finished();
}

void VideoclipRequest::listUsers(const QString &url) {
    setStatus(Loading);
    m_redirects = 0;
    QNetworkRequest request(url);
    request.setRawHeader("User-Agent", USER_AGENT);
    QNetworkReply *reply = networkAccessManager()->get(request);
    connect(reply, SIGNAL(finished()), this, SLOT(checkUsers()));
    connect(this, SIGNAL(finished()), reply, SLOT(deleteLater()));
}

void VideoclipRequest::checkUsers() {
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
    
    const QString response = QString::fromUtf8(reply->readAll());
    const QStringList users = response.split("class=\"item\"");
    QVariantMap result;
    QVariantList items;
    
    for (int i = 1; i < users.size(); i++) {
        const QString &user = users.at(i);
        const QString thumbnailUrl = "http:" + user.section("src=\"", 1, 1).section("\"", 0, 0);
        const QString username = user.section("data-username=\"", 1, 1).section("\"", 0, 0);
        QVariantMap item;
        item["id"] = username;
        item["largeThumbnailUrl"] = thumbnailUrl;
        item["thumbnailUrl"] = thumbnailUrl;
        item["username"] = username;
        items << item;
    }
    
    result["items"] = items;
    
    if ((!items.isEmpty()) && (NEXT_PAGE.indexIn(response) != -1)) {
        result["next"] = NEXT_PAGE.cap(1);
    }
    
    setResult(result);
    setStatus(Ready);
    emit finished();
}

void VideoclipRequest::listStreams(const QString &id) {
    setStatus(Loading);
    m_redirects = 0;
    QNetworkRequest request(STREAM_URL.arg(id));
    request.setRawHeader("User-Agent", USER_AGENT);
    QNetworkReply *reply = networkAccessManager()->get(request);
    connect(reply, SIGNAL(finished()), this, SLOT(checkStreams()));
    connect(this, SIGNAL(finished()), reply, SLOT(deleteLater()));
}

void VideoclipRequest::checkStreams() {
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
    QString streamUrl = response.section("contentURL\" src=\"", 1, 1).section("\"", 0, 0);
    
    if (!streamUrl.isEmpty()) {
        if (streamUrl.startsWith("/")) {
            streamUrl.prepend("http:");
        }
        
        QVariantMap result;
        QVariantList items;
        QVariantMap item;
        item["description"] = "MP4";
        item["id"] = "original";
        item["url"] = streamUrl;
        items << item;
        result["items"] = items;
        setErrorString(QString());
        setResult(result);
        setStatus(Ready);
        emit finished();
    }
    else {
        streamUrl = response.section("youtube.com/embed/", 1, 1).section("\"", 0, 0);

        if (!streamUrl.isEmpty()) {
            youtubeRequest()->list(streamUrl);
        }
        else {
            setErrorString(tr("No streams found"));
            setResult(QVariant());
            setStatus(Failed);
            emit finished();
        }
    }
}

void VideoclipRequest::checkYouTubeStreams() {
    if (m_youtubeRequest->status() == QYouTube::StreamsRequest::Ready) {
        QVariantMap result;
        result["items"] = m_youtubeRequest->result();
        setErrorString(QString());
        setResult(result);
        setStatus(Ready);
    }
    else {
        setErrorString(m_youtubeRequest->errorString());
        setResult(QVariant());
        setStatus(Failed);
    }

    emit finished();
}

QString VideoclipRequest::getRedirect(const QNetworkReply *reply) {
    QString redirect = QString::fromUtf8(reply->rawHeader("Location"));
    
    if (redirect.startsWith("/")) {
        redirect.prepend(reply->url().scheme() + "://" + reply->url().authority());
    }
    
    return redirect;
}

void VideoclipRequest::followRedirect(const QString &url, const char *slot) {
    m_redirects++;
    QNetworkRequest request(url);
    request.setRawHeader("User-Agent", USER_AGENT);
    QNetworkReply *reply = networkAccessManager()->get(request);
    connect(reply, SIGNAL(finished()), this, slot);
    connect(this, SIGNAL(finished()), reply, SLOT(deleteLater()));
}

QNetworkAccessManager* VideoclipRequest::networkAccessManager() {
    return m_nam ? m_nam : m_nam = new QNetworkAccessManager(this);
}

QYouTube::StreamsRequest* VideoclipRequest::youtubeRequest() {
    if (!m_youtubeRequest) {
        m_youtubeRequest = new QYouTube::StreamsRequest(this);
        connect(m_youtubeRequest, SIGNAL(finished()), this, SLOT(checkYouTubeStreams()));
        connect(this, SIGNAL(finished()), m_youtubeRequest, SLOT(cancel()));
    }

    return m_youtubeRequest;
}
