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

#include "pornhubrequest.h"
#include <QNetworkAccessManager>
#include <QNetworkCookieJar>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QRegExp>
#include <QStringList>
#if QT_VERSION >= 0x050000
#include <QUrlQuery>
#endif

const QString PornhubRequest::BASE_URL("http://www.pornhub.com");

const QList<QNetworkCookie> PornhubRequest::MOBILE_COOKIES = QList<QNetworkCookie>()
                                                             << QNetworkCookie("platform", "mobile");
const QList<QNetworkCookie> PornhubRequest::TABLET_COOKIES = QList<QNetworkCookie>()
                                                             << QNetworkCookie("platform", "tablet");

const int PornhubRequest::MAX_REDIRECTS = 8;

PornhubRequest::PornhubRequest(QObject *parent) :
    ResourcesRequest(parent),
    m_nam(0),
    m_status(Null),
    m_redirects(0)
{
}

QString PornhubRequest::errorString() const {
    return m_errorString;
}

void PornhubRequest::setErrorString(const QString &e) {
    m_errorString = e;
}

QVariant PornhubRequest::result() const {
    return m_result;
}

void PornhubRequest::setResult(const QVariant &r) {
    m_result = r;
}

PornhubRequest::Status PornhubRequest::status() const {
    return m_status;
}

void PornhubRequest::setStatus(PornhubRequest::Status s) {
    if (s != status()) {
        m_status = s;
        emit statusChanged(s);
    }
}

bool PornhubRequest::cancel() {
    if (status() == Loading) {
        setStatus(Canceled);
        emit finished();
        return true;
    }

    return false;
}

bool PornhubRequest::get(const QString &resourceType, const QString &resourceId) {
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

bool PornhubRequest::list(const QString &resourceType, const QString &resourceId) {
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
    
    if (resourceType == "stream") {
        listStreams(resourceId);
        return true;
    }

    return false;
}

bool PornhubRequest::search(const QString &resourceType, const QString &query, const QString &order) {
    if ((status() == Loading) || (resourceType.isEmpty()) || (query.isEmpty())) {
        return false;
    }

    if (resourceType == "video") {
        searchVideos(query, order);
        return true;
    }
    
    if (resourceType == "user") {
        searchUsers(query, order);
        return true;
    }

    return false;
}

void PornhubRequest::getVideo(const QString &url) {
    setStatus(Loading);
    m_redirects = 0;
    networkAccessManager()->cookieJar()->setCookiesFromUrl(MOBILE_COOKIES, BASE_URL);
    QNetworkReply *reply = networkAccessManager()->get(QNetworkRequest(url));
    connect(reply, SIGNAL(finished()), this, SLOT(checkVideo()));
    connect(this, SIGNAL(finished()), reply, SLOT(deleteLater()));
}

void PornhubRequest::checkVideo() {
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
    const QString duration = result.section("div class=\"duration thumbOverlay removeWhenPlaying details\">", 1, 1)
                                   .section("<", 0, 0);
    const QString id = reply->url().toString();
    const QString largeThumbnailUrl = result.section("img class=\"mainImage\" src=\"", 1, 1).section("\"", 0, 0);
    const QString thumbnailUrl = result.section("og:image\" content=\"", 1, 1).section("\"", 0, 0);
    const QString title = result.section("div class=\"headerWrap sectionPadding clearfix", 1, 1).section("<h1>", 0, 0);
    const QString user = result.section("div class=\"categoryRow parent clearfix", 1, 1);
    
    QVariantMap video;
    video["duration"] = duration;
    video["id"] = id;
    video["largeThumbnailUrl"] = largeThumbnailUrl;
    video["thumbnailUrl"] = thumbnailUrl;
    video["title"] = title;
    video["url"] = id;
    
    if (!user.isEmpty()) {
        const QString userId = BASE_URL + user.section("href=\"", 1, 1).section("\"", 0, 0);
        const QString username = result.section(">", 1, 1).section("<", 0, 0);
        video["userId"] = userId;
        video["username"] = username;
    }
    
    setResult(video);
    setStatus(Ready);
    emit finished();
}

void PornhubRequest::listVideos(const QString &url) {
    setStatus(Loading);
    m_redirects = 0;
    networkAccessManager()->cookieJar()->setCookiesFromUrl(TABLET_COOKIES, BASE_URL);
    QNetworkReply *reply = networkAccessManager()->get(QNetworkRequest(url));
    connect(reply, SIGNAL(finished()), this, SLOT(checkVideos()));
    connect(this, SIGNAL(finished()), reply, SLOT(deleteLater()));
}

void PornhubRequest::searchVideos(const QString &query, const QString &order) {
    QString url = QString("%1/video/search?search=%2").arg(BASE_URL).arg(query);
    
    if (!order.isEmpty()) {
        url.append("&o=" + order);
    }
    
    listVideos(url);
}

void PornhubRequest::checkVideos() {
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
    const QStringList videos = result.split("<li class=\"videoblock");
    QVariantMap response;
    QVariantList items;
    
    for (int i = 1; i < videos.size(); i++) {
        const QString &video = videos.at(i);
        const QString duration = video.section("class=\"length\">", 1, 1).section("<", 0, 0);
        const QString id = BASE_URL + video.section("href=\"", 1, 1).section("\"", 0, 0);
        const QString largeThumbnailUrl = video.section("data-mediumthumb=\"", 1, 1).section("\"", 0, 0);
        const QString thumbnailUrl = video.section("data-smallthumb=\"", 1, 1).section("\"", 0, 0);
        const QString title = video.section("title=\"", 1, 1).section("\"", 0, 0);
        const int viewCount = qMax(0, video.section("class=\"views\">", 1, 1).section(" ", 0, 0).remove(",").toInt());
        
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
    
    if (items.size() >= 20) {
        response["next"] = incrementPageNumber(reply->url());
    }
    
    setResult(response);
    setStatus(Ready);
    emit finished();
}

void PornhubRequest::getUser(const QString &url) {
    if (url.contains("/channels/")) {
        getChannel(url);
    }
    else if (url.contains("/pornstar/")) {
        getPornstar(url);
    }
    else {
        getMember(url);
    }
}

void PornhubRequest::getChannel(const QString &url) {
    setStatus(Loading);
    m_redirects = 0;
    networkAccessManager()->cookieJar()->setCookiesFromUrl(MOBILE_COOKIES, BASE_URL);
    QNetworkReply *reply = networkAccessManager()->get(QNetworkRequest(url));
    connect(reply, SIGNAL(finished()), this, SLOT(checkChannel()));
    connect(this, SIGNAL(finished()), reply, SLOT(deleteLater()));
}

void PornhubRequest::checkChannel() {
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
            followRedirect(redirect, SLOT(checkChannel()));
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
    const QString description = result.section("div class=\"content\">", 1, 1).remove(QRegExp("<[^>]*>"))
                                                                              .section("<", 0, 0);
    const QString id = reply->url().toString() + "/videos?o=da";
    const QString thumbnailUrl = result.section("div class=\"avatar\">", 1, 1).section("src=\"", 1, 1)
                                                                              .section("\"", 0, 0);
    const QString username = result.section("div class=\"channelName", 1, 1).section("<h3>", 1, 1).section("<", 0, 0);
    
    QVariantMap channel;
    channel["description"] = description;
    channel["id"] = id;
    channel["largeThumbnailUrl"] = thumbnailUrl;
    channel["thumbnailUrl"] = thumbnailUrl;
    channel["username"] = username;
    setResult(channel);
    setStatus(Ready);
    emit finished();
}

void PornhubRequest::getMember(const QString &url) {
    setStatus(Loading);
    m_redirects = 0;
    networkAccessManager()->cookieJar()->setCookiesFromUrl(MOBILE_COOKIES, BASE_URL);
    QNetworkReply *reply = networkAccessManager()->get(QNetworkRequest(url));
    connect(reply, SIGNAL(finished()), this, SLOT(checkMember()));
    connect(this, SIGNAL(finished()), reply, SLOT(deleteLater()));
}

void PornhubRequest::checkMember() {
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
            followRedirect(redirect, SLOT(checkMember()));
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
    const QString description = result.section("div id=\"infoVisible\">", 1, 1).remove(QRegExp("<[^>]*>"))
                                                                              .section("<", 0, 0);
    const QString id = reply->url().toString() + "/videos/public";
    const QString thumbnailUrl = result.section("img id=\"getAvatar\">", 1, 1).section("src=\"", 1, 1)
                                                                              .section("\"", 0, 0);
    const QString username = result.section("div class=\"channelName", 1, 1).section("<h3>", 1, 1).section("<", 0, 0);
    
    QVariantMap member;
    member["description"] = description;
    member["id"] = id;
    member["largeThumbnailUrl"] = thumbnailUrl;
    member["thumbnailUrl"] = thumbnailUrl;
    member["username"] = username;
    setResult(member);
    setStatus(Ready);
    emit finished();
}

void PornhubRequest::getPornstar(const QString &url) {
    setStatus(Loading);
    m_redirects = 0;
    networkAccessManager()->cookieJar()->setCookiesFromUrl(MOBILE_COOKIES, BASE_URL);
    QNetworkReply *reply = networkAccessManager()->get(QNetworkRequest(url));
    connect(reply, SIGNAL(finished()), this, SLOT(checkPornstar()));
    connect(this, SIGNAL(finished()), reply, SLOT(deleteLater()));
}

void PornhubRequest::checkPornstar() {
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
            followRedirect(redirect, SLOT(checkPornstar()));
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
    const QString description = result.section("div id=\"bioBlock\">", 1, 1).remove(QRegExp("<[^>]*>"))
                                                                            .section("<", 0, 0);
    const QString id = reply->url().toString() + "?o=cm";
    const QString thumbnailUrl = result.section("img itemprop=\"contentUrl\"", 1, 1).section("src=\"", 1, 1)
                                                                                    .section("\"", 0, 0);
    const QString username = result.section("span itemprop=\"name\">", 1, 1).section("<", 0, 0).trimmed();
    
    QVariantMap pornstar;
    pornstar["description"] = description;
    pornstar["id"] = id;
    pornstar["largeThumbnailUrl"] = thumbnailUrl;
    pornstar["thumbnailUrl"] = thumbnailUrl;
    pornstar["username"] = username;
    setResult(pornstar);
    setStatus(Ready);
    emit finished();
}

void PornhubRequest::listUsers(const QString &url) {
    if (url.contains("/channels")) {
        listChannels(url);
    }
    else if (url.contains("/pornstars")) {
        listPornstars(url);
    }
    else {
        listMembers(url);
    }
}

void PornhubRequest::searchUsers(const QString &query, const QString &order) {
    const int sep = order.indexOf("_");
    QString type("pornstars");
    QString url = BASE_URL;
    
    if (sep != -1) {
        type = order.left(sep);
    }
    
    if (type == "pornstars") {
        url.append("/pornstars/search?search=" + query);
    }
    else {
        url.append("/user/search?username=" + query);
    }
    
    if (sep < order.size() - 1) {
        url.append("&o=" + order.mid(sep + 1));
    }
    
    listUsers(url);
}

void PornhubRequest::listChannels(const QString &url) {
    setStatus(Loading);
    m_redirects = 0;
    networkAccessManager()->cookieJar()->setCookiesFromUrl(TABLET_COOKIES, BASE_URL);
    QNetworkReply *reply = networkAccessManager()->get(QNetworkRequest(url));
    connect(reply, SIGNAL(finished()), this, SLOT(checkChannels()));
    connect(this, SIGNAL(finished()), reply, SLOT(deleteLater()));
}

void PornhubRequest::checkChannels() {
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
            followRedirect(redirect, SLOT(checkChannels()));
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
    const QStringList users = result.section("div class=\"channelsContainer", 1, 1).split("li class=\"wrap");
    QVariantMap response;
    QVariantList items;
    
    for (int i = 1; i < users.size(); i++) {
        const QString &user = users.at(i);
        const QString id = QString("%1%2/videos?o=da").arg(BASE_URL)
                                                      .arg(user.section("href=\"", 1, 1).section("\"", 0, 0));
        const QString thumbnailUrl = user.section("src=\"", 2, 2).section("\"", 0, 0);
        const QString username = user.section("class=\"usernameLink\">", 1, 1).section("<", 0, 0);
        
        QVariantMap item;
        item["id"] = id;
        item["largeThumbnailUrl"] = thumbnailUrl;
        item["thumbnailUrl"] = thumbnailUrl;
        item["username"] = username;
        items << item;
    }
    
    response["items"] = items;
    
    if (items.size() >= 36) {
        response["next"] = incrementPageNumber(reply->url());
    }
    
    setResult(response);
    setStatus(Ready);
    emit finished();
}

void PornhubRequest::listMembers(const QString &url) {
    setStatus(Loading);
    m_redirects = 0;
    networkAccessManager()->cookieJar()->setCookiesFromUrl(TABLET_COOKIES, BASE_URL);
    QNetworkReply *reply = networkAccessManager()->get(QNetworkRequest(url));
    connect(reply, SIGNAL(finished()), this, SLOT(checkMembers()));
    connect(this, SIGNAL(finished()), reply, SLOT(deleteLater()));
}

void PornhubRequest::checkMembers() {
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
            followRedirect(redirect, SLOT(checkMembers()));
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
    const QStringList users = result.section("ul class=\"userWidgetWrapperGrid", -1).section("</ul>", 0, 0).split("<li>");
    QVariantMap response;
    QVariantList items;
    
    for (int i = 1; i < users.size(); i++) {
        const QString &user = users.at(i);
        const QString id = QString("%1%2/videos/public").arg(BASE_URL)
                                                        .arg(user.section("href=\"", 1, 1).section("\"", 0, 0));
        const QString thumbnailUrl = user.section("src=\"", 1, 1).section("\"", 0, 0);
        const QString username = user.section("alt=\"", 1, 1).section("\"", 0, 0);
        
        QVariantMap item;
        item["id"] = id;
        item["largeThumbnailUrl"] = thumbnailUrl;
        item["thumbnailUrl"] = thumbnailUrl;
        item["username"] = username;
        items << item;
    }
    
    response["items"] = items;
    
    if (items.size() >= 42) {
        response["next"] = incrementPageNumber(reply->url());
    }
    
    setResult(response);
    setStatus(Ready);
    emit finished();
}

void PornhubRequest::listPornstars(const QString &url) {
    setStatus(Loading);
    m_redirects = 0;
    networkAccessManager()->cookieJar()->setCookiesFromUrl(TABLET_COOKIES, BASE_URL);
    QNetworkReply *reply = networkAccessManager()->get(QNetworkRequest(url));
    connect(reply, SIGNAL(finished()), this, SLOT(checkPornstars()));
    connect(this, SIGNAL(finished()), reply, SLOT(deleteLater()));
}

void PornhubRequest::checkPornstars() {
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
            followRedirect(redirect, SLOT(checkPornstars()));
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
    const QStringList users = result.section("ul class=\"pornstarListing", -1).section("</ul>", 0, 0).split("<li>");
    QVariantMap response;
    QVariantList items;
    
    for (int i = 1; i < users.size(); i++) {
        const QString &user = users.at(i);
        const QString id = QString("%1%2?o=cm").arg(BASE_URL).arg(user.section("href=\"", 1, 1).section("\"", 0, 0));
        const QString thumbnailUrl = user.section("src=\"", 1, 1).section("\"", 0, 0);
        const QString username = user.section("alt=\"", 1, 1).section("\"", 0, 0);
        
        QVariantMap item;
        item["id"] = id;
        item["largeThumbnailUrl"] = thumbnailUrl;
        item["thumbnailUrl"] = thumbnailUrl;
        item["username"] = username;
        items << item;
    }
    
    response["items"] = items;
    
    if (items.size() >= 18) {
        response["next"] = incrementPageNumber(reply->url());
    }
    
    setResult(response);
    setStatus(Ready);
    emit finished();
}

void PornhubRequest::listCategories(const QString &url) {
    setStatus(Loading);
    m_redirects = 0;
    networkAccessManager()->cookieJar()->setCookiesFromUrl(MOBILE_COOKIES, BASE_URL);
    QNetworkReply *reply = networkAccessManager()->get(QNetworkRequest(url));
    connect(reply, SIGNAL(finished()), this, SLOT(checkCategories()));
    connect(this, SIGNAL(finished()), reply, SLOT(deleteLater()));
}

void PornhubRequest::checkCategories() {
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
    const QStringList categories = result.section("<ul id=\"fullList\"", 1, 1).section("</ul>", 0, 0).split("<li>");
    QVariantMap response;
    QVariantList items;
    
    for (int i = 1; i < categories.size(); i++) {
        const QString &category = categories.at(i);
        const QString id = QString("%1%2&o=cm").arg(BASE_URL)
                                               .arg(category.section("href=\"", 1, 1).section("\"", 0, 0));
        const QString title = category.section("alt=\"", 1, 1).section("\"", 0, 0);
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

void PornhubRequest::listStreams(const QString &url) {
    setStatus(Loading);
    m_redirects = 0;
    networkAccessManager()->cookieJar()->setCookiesFromUrl(MOBILE_COOKIES, BASE_URL);
    QNetworkReply *reply = networkAccessManager()->get(QNetworkRequest(url));
    connect(reply, SIGNAL(finished()), this, SLOT(checkStreams()));
    connect(this, SIGNAL(finished()), reply, SLOT(deleteLater()));
}

void PornhubRequest::checkStreams() {
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
    
    const QString result = QString::fromUtf8(reply->readAll());
    const QString description("MP4 audio/video");
    const QRegExp re("player_quality_(\\d+)p = '([^']+)");
    int pos = 0;
    
    QVariantMap response;
    QVariantList streams;
    
    while ((pos = re.indexIn(result, pos)) != -1) {
        const QString id = re.cap(1);
        const int height = id.toInt();
        const QString url = re.cap(2);
        
        QVariantMap stream;
        stream["description"] = description;
        stream["height"] = height;
        stream["id"] = id;
        stream["url"] = url;
        streams << stream;
        
        pos += re.matchedLength();
    }
    
    response["items"] = streams;
    setResult(response);
    setStatus(Ready);
    emit finished();
}

QString PornhubRequest::getRedirect(const QNetworkReply *reply) {
    QString redirect = QString::fromUtf8(reply->rawHeader("Location"));
    
    if (redirect.startsWith("/")) {
        redirect.prepend(reply->url().scheme() + "://" + reply->url().authority());
    }
    
    return redirect;
}

QUrl PornhubRequest::incrementPageNumber(QUrl url) {
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

void PornhubRequest::followRedirect(const QString &url, const char *slot) {
    m_redirects++;
    QNetworkReply *reply = networkAccessManager()->get(QNetworkRequest(url));
    connect(reply, SIGNAL(finished()), this, slot);
}

QNetworkAccessManager* PornhubRequest::networkAccessManager() {
    return m_nam ? m_nam : m_nam = new QNetworkAccessManager(this);
}
