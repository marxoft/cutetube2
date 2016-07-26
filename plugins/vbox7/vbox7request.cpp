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

#include "vbox7request.h"
#include <QDateTime>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QStringList>

const QString Vbox7Request::BASE_URL("http://vbox7.com");
const QString Vbox7Request::SEARCH_URL(BASE_URL + "/search/combined_search.do");

const QRegExp Vbox7Request::DURATION("T((\\d+)H|)((\\d+)M|)((\\d+)S|)");
const QRegExp Vbox7Request::NO_JS_ERROR("/show:missjavascript\\?[^']+");

const int Vbox7Request::MAX_REDIRECTS = 8;

Vbox7Request::Vbox7Request(QObject *parent) :
    ResourcesRequest(parent),
    m_nam(0),
    m_status(Null),
    m_redirects(0)
{
}

QString Vbox7Request::errorString() const {
    return m_errorString;
}

void Vbox7Request::setErrorString(const QString &e) {
    m_errorString = e;
}

QVariant Vbox7Request::result() const {
    return m_result;
}

void Vbox7Request::setResult(const QVariant &r) {
    m_result = r;
}

Vbox7Request::Status Vbox7Request::status() const {
    return m_status;
}

void Vbox7Request::setStatus(Vbox7Request::Status s) {
    if (s != status()) {
        m_status = s;
        emit statusChanged(s);
    }
}

bool Vbox7Request::cancel() {
    if (status() == Loading) {
        setStatus(Canceled);
        emit finished();
        return true;
    }

    return false;
}

bool Vbox7Request::get(const QString &resourceType, const QString &resourceId) {
    if ((status() == Loading) || (resourceId.isEmpty())) {
        return false;
    }
    
    if (resourceType == "user") {
        getUser(resourceId);
        return true;
    }

    if (resourceType == "video") {
        getVideo(resourceId);
        return true;
    }

    return false;
}

bool Vbox7Request::list(const QString &resourceType, const QString &resourceId) {
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

bool Vbox7Request::search(const QString &resourceType, const QString &query, const QString &order) {
    if ((status() == Loading) || (resourceType.isEmpty()) || (query.isEmpty())) {
        return false;
    }

    if (resourceType == "video") {
        searchVideos(query, order);
        return true;
    }

    return false;
}

void Vbox7Request::getUser(const QString &url) {
    setStatus(Loading);
    m_url = url;
    m_redirects = 0;
    QNetworkReply *reply = networkAccessManager()->get(QNetworkRequest(m_url));
    connect(reply, SIGNAL(finished()), this, SLOT(checkUser()));
    connect(this, SIGNAL(finished()), reply, SLOT(deleteLater()));
}

void Vbox7Request::checkUser() {
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
    redirect = getRedirect(result, m_url);
    
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
    
    if (result.isEmpty()) {
        setErrorString(tr("No user info found"));
        setResult(QVariant());
        setStatus(Failed);
    }
    else {
        const QString description = result.section("<div id=\"descCont\" >", 1, 1).section("</div>", 0, 0).trimmed();
        const QString info = result.section("<header class=\"user-header\"", 1, 1).section("</header>", 0, 0);
        const QString thumbnailUrl = "http:" + info.section("src=\"", 1, 1).section("\"", 0, 0);
        const QUrl url = reply->url();
        const QString username = info.section("/user:", 1, 1).section("\"", 0, 0);
        
        QVariantMap user;
        user["description"] = description;
        user["id"] = url;
        user["largeThumbnailUrl"] = thumbnailUrl;
        user["thumbnailUrl"] = thumbnailUrl;
        user["url"] = url;
        user["username"] = username;
        setErrorString(QString());
        setResult(user);
        setStatus(Ready);
    }
    
    emit finished();
}

void Vbox7Request::getVideo(const QString &url) {
    setStatus(Loading);
    m_url = url;
    m_redirects = 0;
    QNetworkReply *reply = networkAccessManager()->get(QNetworkRequest(m_url));
    connect(reply, SIGNAL(finished()), this, SLOT(checkVideo()));
    connect(this, SIGNAL(finished()), reply, SLOT(deleteLater()));
}

void Vbox7Request::checkVideo() {
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
    redirect = getRedirect(result, m_url);
    
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
    
    if (result.isEmpty()) {
        setErrorString(tr("No video info found"));
        setResult(QVariant());
        setStatus(Failed);
    }
    else {
        const QString date = QDateTime::fromString(result.section("itemprop=\"uploadDate\" content=\"", 1, 1)
                                                         .section("\"", 0, 0), Qt::ISODate).toString("dd/MM/yyyy");
        const QString description = result.section("name=\"description\" content=\"", 1, 1).section("\"", 0, 0);
        const QString duration = formatDuration(result.section("itemprop=\"duration\" content=\"", 1, 1)
                                                      .section("\"", 0, 0));
        const QString largeThumbnailUrl = "http:" + result.section("itemprop=\"thumbnailUrl\" content=\"", 1, 1)
                                                          .section("\"", 0, 0);
        const QString thumbnailUrl = "http:" + result.section("name=\"thumbnail\" content=\"", 1, 1)
                                                     .section("\"", 0, 0);
        const QString title = result.section("name=\"title\" content=\"", 1, 1).section("\"", 0, 0);
        const QUrl url = reply->url();
        const QString user = result.section("class=\"avatar-img\"", 1, 1).section(">", 0, 0);
        const QString userId = BASE_URL + user.section("href=\"", 1, 1).section("\"", 0, 0);
        const QString username = user.section("title=\"", 1, 1).section("\"", 0, 0);
        const int viewCount = qMax(0, result.section("itemprop=\"InteractionCount\" >", 1, 1)
                                            .section("<", 0, 0).trimmed().remove(" ").toInt());
        
        QVariantMap video;
        video["date"] = date;
        video["description"] = description;
        video["duration"] = duration;
        video["id"] = url;
        video["largeThumbnailUrl"] = largeThumbnailUrl;
        video["thumbnailUrl"] = thumbnailUrl;
        video["title"] = title;
        video["url"] = url;
        video["userId"] = userId;
        video["username"] = username;
        video["viewCount"] = viewCount;
        setErrorString(QString());
        setResult(video);
        setStatus(Ready);
    }
    
    emit finished();
}

void Vbox7Request::listVideos(const QString &url) {
    setStatus(Loading);
    m_url = url.contains(QRegExp("/user:\\w+$")) ? url + "?p=allvideos" : url;
    m_redirects = 0;
    QNetworkReply *reply =
    networkAccessManager()->get(QNetworkRequest(m_url));
    connect(reply, SIGNAL(finished()), this, SLOT(checkVideos()));
    connect(this, SIGNAL(finished()), reply, SLOT(deleteLater()));
}

void Vbox7Request::searchVideos(const QString &query, const QString &order) {
    listVideos(QString("%1?type=video&q=%2&order=%3").arg(SEARCH_URL).arg(query).arg(order));
}

void Vbox7Request::checkVideos() {
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
    redirect = getRedirect(result, m_url);
    
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
    
    const QStringList videos = result.split("<div class=\"clip-thumb");
    QVariantMap response;
    QVariantList items;
    
    for (int i = 1; i < videos.size(); i++) {
        const QString &video = videos.at(i);
        const QString date = video.section("<p>", 1, 1).section("<", 0, 0).trimmed().replace(".", "/");
        const QString duration = video.section("class=\"thumb-time\"><span>", 1, 1).section("<", 0, 0).trimmed();
        const QString id = BASE_URL + video.section("href=\"", 1, 1).section("\"", 0, 0);
        const QString thumbnailUrl = "http:" + video.section("src=\"", 1, 1).section("\"", 0, 0);
        const QString title = video.section("alt=\"", 1, 1).section("\"", 0, 0);
        const QString user = video.section("class=\"clip-user", 1, 1).section("</a>", 0, 0);
        const QString userId = BASE_URL + user.section("href=\"", -1).section("\"", 0, 0);
        const QString username = user.section(">", -1);
        const int viewCount = qMax(0, video.section("class=\"icon-eye\"></i>", 1, 1)
                                           .section("<", 0, 0).trimmed().remove("&nbsp;").remove(" ").toInt());
        
        QVariantMap item;
        item["date"] = date;
        item["duration"] = duration;
        item["id"] = id;
        item["largeThumbnailUrl"] = thumbnailUrl;
        item["thumbnailUrl"] = thumbnailUrl;
        item["title"] = title;
        item["url"] = id;
        item["userId"] = userId;
        item["username"] = username;
        item["viewCount"] = viewCount;
        items << item;
    }
    
    response["items"] = items;
    
    const QString next = result.section("class=\"nextPage\"><a href=\"", 1, 1).section("\"", 0, 0);
    
    if (!next.isEmpty()) {            
        response["next"] = BASE_URL + next;
    }
    
    setErrorString(QString());
    setResult(response);
    setStatus(Ready);
    emit finished();
}

void Vbox7Request::listStreams(const QString &url) {
    setStatus(Loading);
    m_url = url;
    m_redirects = 0;
    QNetworkReply *reply = networkAccessManager()->get(QNetworkRequest(url));
    connect(reply, SIGNAL(finished()), this, SLOT(checkStreams()));
    connect(this, SIGNAL(finished()), reply, SLOT(deleteLater()));
}

void Vbox7Request::checkStreams() {
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
    
    const QString result = QString::fromUtf8(reply->readAll());
    redirect = getRedirect(result, m_url);
    
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
    
    const QString options = result.section("var options =", 1, 1).section(";\n", 0, 0);
    QString url = options.section("src: '", 1, 1).section("'", 0, 0);
    QVariantMap response;
    QVariantList streams;
    
    if (!url.isEmpty()) {
        if (url.startsWith("//")) {
            url.prepend("http:");
        }
        
        const bool isHD = options.section("videoIsHD: ", 1, 1).section(",", 0, 0).toInt() == 1;
        const bool hasLD = options.section("videoHasLD: ", 1, 1).section(",", 0, 0).toInt() == 1;
        
        if (isHD) {
            QVariantMap hd;
            hd["description"] = "MP4 audio/video";
            hd["ext"] = "mp4";
            hd["height"] = 720;
            hd["id"] = "hd";
            hd["url"] = url;
            hd["width"] = 1280;
            streams << hd;
        }
        
        if ((hasLD) || (!isHD)) {
            const int dot = url.lastIndexOf(".");
            QVariantMap ld;
            ld["description"] = "MP4 audio/video";
            ld["ext"] = "mp4";
            ld["height"] = 480;
            ld["id"] = "ld";
            ld["url"] = (isHD ? QString("%1_480p%2").arg(url.left(dot)).arg(url.mid(dot)) : url);
            ld["width"] = 852;
            streams << ld;
        }
    }
        
    response["items"] = streams;
    setErrorString(QString());
    setResult(response);
    setStatus(Ready);
    emit finished();
}

QString Vbox7Request::formatDuration(const QString &duration) {
    if (DURATION.indexIn(duration) != -1) {
        const int hours = qMax(0, DURATION.cap(2).toInt());
        const int mins = qMax(0, DURATION.cap(4).toInt());
        const int secs = qMax(0, DURATION.cap(6).toInt());
        
        if (hours > 0) {
            return QString("%1:%2:%3").arg(hours, 2, 10, QChar('0'))
                                      .arg(mins, 2, 10, QChar('0'))
                                      .arg(secs, 2, 10, QChar('0'));
        }
        
        return QString("%1:%2").arg(mins, 2, 10, QChar('0')).arg(secs, 2, 10, QChar('0'));
    }
    
    return QString("00:00");
}

QString Vbox7Request::getRedirect(const QNetworkReply *reply) {
    QString redirect = QString::fromUtf8(QByteArray::fromPercentEncoding(reply->rawHeader("Location")));
    
    if (redirect.startsWith("/")) {
        redirect.prepend(reply->url().scheme() + "://" + reply->url().authority());
    }
    
    return redirect;
}

QString Vbox7Request::getRedirect(const QString &response, const QString &url) {
    if (response == "1") {
        return url;
    }
    
    if (NO_JS_ERROR.indexIn(response) != -1) {
        return BASE_URL + NO_JS_ERROR.cap();
    }
    
    return QString();
}

void Vbox7Request::followRedirect(const QString &url, const char *slot) {
    m_redirects++;
    QNetworkReply *reply = networkAccessManager()->get(QNetworkRequest(url));
    connect(reply, SIGNAL(finished()), this, slot);
    connect(this, SIGNAL(finished()), reply, SLOT(deleteLater()));
}

QNetworkAccessManager* Vbox7Request::networkAccessManager() {
    return m_nam ? m_nam : m_nam = new QNetworkAccessManager(this);
}
