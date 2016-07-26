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

#include "xhamsterrequest.h"
#include "json.h"
#include <QDateTime>
#include <QNetworkAccessManager>
#include <QNetworkCookie>
#include <QNetworkCookieJar>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QStringList>
#if QT_VERSION >= 0x050000
#include <QUrlQuery>
#endif

const QString XHamsterRequest::BASE_URL("http://xhamster.com");

const QByteArray XHamsterRequest::USER_AGENT("Wget/1.13.4 (linux-gnu)");

const QRegExp XHamsterRequest::PAGE_REGEXP("new-(\\d+)\\.html$");

const int XHamsterRequest::MAX_REDIRECTS = 8;

XHamsterRequest::XHamsterRequest(QObject *parent) :
    ResourcesRequest(parent),
    m_nam(0),
    m_status(Null),
    m_redirects(0)
{
}

QString XHamsterRequest::errorString() const {
    return m_errorString;
}

void XHamsterRequest::setErrorString(const QString &e) {
    m_errorString = e;
}

QVariant XHamsterRequest::result() const {
    return m_result;
}

void XHamsterRequest::setResult(const QVariant &r) {
    m_result = r;
}

XHamsterRequest::Status XHamsterRequest::status() const {
    return m_status;
}

void XHamsterRequest::setStatus(XHamsterRequest::Status s) {
    if (s != status()) {
        m_status = s;
        emit statusChanged(s);
    }
}

bool XHamsterRequest::cancel() {
    if (status() == Loading) {
        setStatus(Canceled);
        emit finished();
        return true;
    }

    return false;
}

bool XHamsterRequest::get(const QString &resourceType, const QString &resourceId) {
    if ((status() == Loading) || (resourceId.isEmpty())) {
        return false;
    }

    if (resourceType == "video") {
        getVideo(resourceId);
        return true;
    }

    return false;
}

bool XHamsterRequest::list(const QString &resourceType, const QString &resourceId) {
    if ((status() == Loading) || (resourceId.isEmpty())) {
        return false;
    }

    if (resourceType == "video") {
        listVideos(resourceId);
        return true;
    }

    if (resourceType == "category") {
        listCategories(resourceId);
        return true;
    }
    
    if (resourceType == "stream") {
        listStreams(resourceId);
        return true;
    }

    return false;
}

bool XHamsterRequest::search(const QString &resourceType, const QString &query, const QString &order) {
    if ((status() == Loading) || (resourceType.isEmpty()) || (query.isEmpty())) {
        return false;
    }

    if (resourceType == "video") {
        searchVideos(query, order);
        return true;
    }

    return false;
}

void XHamsterRequest::getVideo(const QString &url) {
    setStatus(Loading);
    m_redirects = 0;
    QNetworkRequest request(url);
    request.setRawHeader("User-Agent", USER_AGENT);
    QNetworkReply *reply = networkAccessManager()->get(request);
    connect(reply, SIGNAL(finished()), this, SLOT(checkVideo()));
    connect(this, SIGNAL(finished()), reply, SLOT(deleteLater()));
}

void XHamsterRequest::checkVideo() {
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
    const QString date = QDateTime::fromString(result.section("itemprop=\"datePublished uploadDate\" content=\"", 1, 1)
                                                     .section("\"", 0, 0), "yyyy-MM-dd").toString("dd/MM/yyyy");
    const QString duration = result.section("Runtime:</span>", 1, 1).section("<", 0, 0).trimmed();
    const QString id = reply->url().toString();
    const QString thumbnailUrl = result.section("itemprop=\"thumbnailUrl\" href=\"", 1, 1).section("\"", 0, 0);
    const QString title = result.section("<h1 itemprop=\"name\">", 1, 1).section("<", 0, 0);
    const int viewCount = qMax(0, result.section("itemprop=\"interactionCount\" content=\"UserPlays:", 1, 1)
                                        .section("\"", 0, 0).toInt());
    
    QVariantMap video;
    video["date"] = date;
    video["duration"] = duration;
    video["id"] = id;
    video["largeThumbnailUrl"] = thumbnailUrl;
    video["thumbnailUrl"] = thumbnailUrl;
    video["title"] = title;
    video["url"] = id;
    video["viewCount"] = viewCount;
    
    setResult(video);
    setStatus(Ready);
    emit finished();
}

void XHamsterRequest::listVideos(const QString &url) {
    setStatus(Loading);
    m_redirects = 0;
    QNetworkRequest request(url);
    request.setRawHeader("User-Agent", USER_AGENT);
    QNetworkReply *reply = networkAccessManager()->get(request);
    connect(reply, SIGNAL(finished()), this, SLOT(checkVideos()));
    connect(this, SIGNAL(finished()), reply, SLOT(deleteLater()));
}

void XHamsterRequest::searchVideos(const QString &query, const QString &order) {
    const QNetworkCookie cookie("search_video",
    QString("{\"sort\":\"%1\",\"duration\":\"\",\"channels\":false,\"quality\":0,\"date\":\"\"}")
           .arg(order).toUtf8());
    networkAccessManager()->cookieJar()->setCookiesFromUrl(QList<QNetworkCookie>() << cookie, BASE_URL);
    listVideos(QString("%1/search.php?qcat=video&q=%2").arg(BASE_URL).arg(query));
}

void XHamsterRequest::checkVideos() {
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
    const QStringList videos = result.split("<div class=\"video\"");
    QVariantMap response;
    QVariantList items;
    
    for (int i = 1; i < videos.size(); i++) {
        const QString &video = videos.at(i);
        const QString duration = video.section("<b>", 1, 1).section("<", 0, 0);
        const QString id = video.section("href=\"", 1, 1).section("\"", 0, 0);
        const QString thumbnailUrl = video.section("img src='", 1, 1).section("'", 0, 0);
        const QString title = video.section("alt=\"", 1, 1).section("\"", 0, 0);
        
        const int viewCount = qMax(0, video.section(video.contains("class=\"views-value\">")
                                   ? "class=\"views-value\">" : "itemprop=\"interactionCount\">", 1, 1)
                                   .section("<", 0, 0).remove(",").toInt());
        
        QVariantMap item;
        item["duration"] = duration;
        item["id"] = id;
        item["largeThumbnailUrl"] = thumbnailUrl;
        item["thumbnailUrl"] = thumbnailUrl;
        item["title"] = title;
        item["url"] = id;
        item["viewCount"] = viewCount;
        items << item;
    }
    
    response["items"] = items;
    
    if (items.size() >= 28) {
        response["next"] = incrementPageNumber(reply->url());
    }
    
    setResult(response);
    setStatus(Ready);
    emit finished();
}

void XHamsterRequest::listCategories(const QString &url) {
    setStatus(Loading);
    m_redirects = 0;
    QNetworkRequest request(url);
    request.setRawHeader("User-Agent", USER_AGENT);
    QNetworkReply *reply = networkAccessManager()->get(request);
    connect(reply, SIGNAL(finished()), this, SLOT(checkCategories()));
    connect(this, SIGNAL(finished()), reply, SLOT(deleteLater()));
}

void XHamsterRequest::checkCategories() {
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
    const QStringList letters = result.section("<div id=\"footer\">", 0, 0).split("<div class=\"letter-categories\">");
    QVariantMap response;
    QVariantList items;
    
    for (int i = 1; i < letters.size(); i++) {
        const QStringList categories = letters.at(i).split("<a ");
        
        for (int j = 1; j < categories.size(); j++) {
            const QString &category = categories.at(j);
            const QString id = category.section("href=\"", 1, 1).section("\"", 0, 0);
            const QString title = category.section(">", 1, 1).section("<", 0, 0);
            
            QVariantMap item;
            item["id"] = id;
            item["title"] = title;
            items << item;
        }
    }
    
    response["items"] = items;
    setResult(response);
    setStatus(Ready);
    emit finished();
}

void XHamsterRequest::listStreams(const QString &url) {
    setStatus(Loading);
    m_redirects = 0;
    QNetworkRequest request(url);
    request.setRawHeader("User-Agent", USER_AGENT);
    QNetworkReply *reply = networkAccessManager()->get(request);
    connect(reply, SIGNAL(finished()), this, SLOT(checkStreams()));
    connect(this, SIGNAL(finished()), reply, SLOT(deleteLater()));
}

void XHamsterRequest::checkStreams() {
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
    
    const QString vars = QString::fromUtf8(reply->readAll()).section("vars: ", 1, 1).section(",\n", 0, 0);
    const QVariantMap urls = QtJson::Json::parse(QtJson::Json::parse(vars).toMap().value("new").toMap()
                                                                          .value("videoUrls").toString()).toMap();
    const QString description("MP4 audio/video");    
    QVariantMap response;
    QVariantList streams;
    
    QMapIterator<QString, QVariant> iterator(urls);
    
    while (iterator.hasNext()) {
        iterator.next();
        const QVariantList l = iterator.value().toList();
        
        if (!l.isEmpty()) {
            const QString &key = iterator.key();
            const int height = key.left(key.indexOf("p")).toInt();
            
            QVariantMap stream;
            stream["description"] = description;
            stream["height"] = height;
            stream["id"] = key;
            stream["url"] = l.first();
            streams << stream;
        }
    }
    
    response["items"] = streams;
    setResult(response);
    setStatus(Ready);
    emit finished();
}

QString XHamsterRequest::getRedirect(const QNetworkReply *reply) {
    QString redirect = QString::fromUtf8(reply->rawHeader("Location"));
    
    if (redirect.startsWith("/")) {
        redirect.prepend(reply->url().scheme() + "://" + reply->url().authority());
    }
    
    return redirect;
}

QUrl XHamsterRequest::incrementPageNumber(QUrl url) {
    QString u = url.toString();
    const int pos = PAGE_REGEXP.lastIndexIn(u);
    
    if (pos != -1) {
        QString match = PAGE_REGEXP.cap(0);
        const QString page = PAGE_REGEXP.cap(1);
        url.setUrl(u.replace(match, match.replace(page, QString::number(page.toInt() + 1))));
        return url;
    }
#if QT_VERSION >= 0x050000
    QUrlQuery query(url);
    const int page = qMax(2, query.queryItemValue("page").toInt() + 1);
    query.removeQueryItem("page");
    query.addQueryItem("page", QString::number(page));
    url.setQuery(query);
#else
    const int page = qMax(2, url.queryItemValue("page").toInt() + 1);
    url.removeQueryItem("page");
    url.addQueryItem("page", QString::number(page));
#endif
    return url;
}

void XHamsterRequest::followRedirect(const QString &url, const char *slot) {
    m_redirects++;
    QNetworkRequest request(url);
    request.setRawHeader("User-Agent", USER_AGENT);
    QNetworkReply *reply = networkAccessManager()->get(request);
    connect(reply, SIGNAL(finished()), this, slot);
    connect(this, SIGNAL(finished()), reply, SLOT(deleteLater()));
}

QNetworkAccessManager* XHamsterRequest::networkAccessManager() {
    return m_nam ? m_nam : m_nam = new QNetworkAccessManager(this);
}
