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

#include "tvrequest.h"
#include "json.h"
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#if QT_VERSION >= 0x050000
#include <QUrlQuery>
#endif

const QString TVRequest::API_URL("http://marxoft.co.uk/api/cutetv");
const QString TVRequest::LOGO_URL("http://static.filmon.com/assets/channels/%1/big_logo.png");
const QString TVRequest::STREAMS_URL("http://www.filmon.tv/api-v2/channel");

const int TVRequest::MAX_REDIRECTS = 8;

TVRequest::TVRequest(QObject *parent) :
    ResourcesRequest(parent),
    m_nam(0),
    m_status(Null),
    m_redirects(0)
{
}

QString TVRequest::errorString() const {
    return m_errorString;
}

void TVRequest::setErrorString(const QString &e) {
    m_errorString = e;
}

QVariant TVRequest::result() const {
    return m_result;
}

void TVRequest::setResult(const QVariant &r) {
    m_result = r;
}

TVRequest::Status TVRequest::status() const {
    return m_status;
}

void TVRequest::setStatus(TVRequest::Status s) {
    if (s != status()) {
        m_status = s;
        emit statusChanged(s);
    }
}

bool TVRequest::cancel() {
    if (status() == Loading) {
        setStatus(Canceled);
        emit finished();
        return true;
    }

    return false;
}

bool TVRequest::list(const QString &resourceType, const QString &resourceId) {
    if ((status() == Loading) || (resourceId.isEmpty())) {
        return false;
    }

    if (resourceType == "video") {
        listChannels(resourceId);
        return true;
    }

    if (resourceType == "category") {
        listCategories(resourceId);
        return true;
    }
    
    if (resourceType == "stream") {
        m_streams.clear();
        listStreams(resourceId, "hls");
        return true;
    }

    return false;
}

bool TVRequest::search(const QString &resourceType, const QString &query, const QString &order) {
    if ((status() == Loading) || (resourceType.isEmpty()) || (query.isEmpty())) {
        return false;
    }

    if (resourceType == "video") {
        searchChannels(query, order);
        return true;
    }

    return false;
}

void TVRequest::listChannels(const QString &url) {
    setStatus(Loading);
    m_redirects = 0;
    QNetworkReply *reply = networkAccessManager()->get(QNetworkRequest(url));
    connect(reply, SIGNAL(finished()), this, SLOT(checkChannels()));
    connect(this, SIGNAL(finished()), reply, SLOT(deleteLater()));
}

void TVRequest::searchChannels(const QString &query, const QString &order) {
    listChannels(QString("%1/channels?search=%2&sort=%3").arg(API_URL).arg(query).arg(order));
}

void TVRequest::checkChannels() {
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
    
    const QVariantMap response = QtJson::Json::parse(QString::fromUtf8(reply->readAll())).toMap();
    const QVariantList channels = response.value("items").toList();
    QVariantMap result;
    QVariantList items;
    
    foreach (const QVariant &c, channels) {
        QVariantMap item = c.toMap();
        const QString logo = LOGO_URL.arg(item.value("id").toString());
        item["downloadable"] = false;
        item["largeThumbnailUrl"] = logo;
        item["relatedVideosId"] = QString("%1/channels?genre=%2&sort=title").arg(API_URL)
                                                                            .arg(item.value("genre").toString());
        item["thumbnailUrl"] = logo;
        item["url"] = item.value("id");
        items << item;
    }
    
    result["items"] = items;
    
    if (response.contains("next")) {
        result["next"] = API_URL + response.value("next").toString();
    }
    
    setErrorString(QString());
    setResult(result);
    setStatus(Ready);
    emit finished();
}

void TVRequest::listCategories(const QString &url) {
    setStatus(Loading);
    m_redirects = 0;
    QNetworkReply *reply = networkAccessManager()->get(QNetworkRequest(url));
    connect(reply, SIGNAL(finished()), this, SLOT(checkCategories()));
    connect(this, SIGNAL(finished()), reply, SLOT(deleteLater()));
}

void TVRequest::checkCategories() {
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
    
    const QVariantMap response = QtJson::Json::parse(QString::fromUtf8(reply->readAll())).toMap();
    const QVariantList categories = response.value("items").toList();
    QString categoryType = reply->url().path().section("/", -1);
    categoryType.chop(1);
    QVariantMap result;
    QVariantList items;
    
    foreach (const QVariant &c, categories) {
        QVariantMap item = c.toMap();
        item["videosId"] = QString("%1/channels?%2=%3&sort=title").arg(API_URL).arg(categoryType)
                                                                  .arg(item.value("title").toString());
        items << item;
    }
    
    result["items"] = items;
    
    if (response.contains("next")) {
        result["next"] = API_URL + response.value("next").toString();
    }
    
    setErrorString(QString());
    setResult(result);
    setStatus(Ready);
    emit finished();
}

void TVRequest::listStreams(const QString &id, const QString &protocol) {
    setStatus(Loading);
    m_redirects = 0;
    QNetworkReply *reply =
    networkAccessManager()->get(QNetworkRequest(QString("%1/%2?protocol=%3").arg(STREAMS_URL)
                                                                            .arg(id)
                                                                            .arg(protocol)));
    connect(reply, SIGNAL(finished()), this, SLOT(checkStreams()));
    connect(this, SIGNAL(finished()), reply, SLOT(deleteLater()));
}

void TVRequest::checkStreams() {
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
    const QVariantMap channel = QtJson::Json::parse(response).toMap().value("data").toMap();
#if QT_VERSION >= 0x050000
    const QString protocol = QUrlQuery(reply->url()).queryItemValue("protocol");
#else
    const QString protocol = reply->url().queryItemValue("protocol");
#endif
    if (!channel.isEmpty()) {
        const QVariantList streams = channel.value("streams").toList();
        
        foreach (const QVariant &v, streams) {
            const QVariantMap s = v.toMap();
            const QString quality = s.value("quality").toString();
            QVariantMap stream;
            stream["id"] = QString("%1_%2").arg(protocol).arg(quality);
            stream["description"] = QString("%1 %2%3").arg(protocol.toUpper()).arg(quality.left(1).toUpper())
                                                      .arg(quality.mid(1));
            stream["url"] = s.value("url");
            m_streams << stream;
        }
    }
    
    if (protocol == "rtsp") {
        if (m_streams.isEmpty()) {
            setErrorString(tr("No streams found"));
            setResult(QVariant());
            setStatus(Failed);
        }
        else {
            QVariantMap result;
            result["items"] = m_streams;
            setErrorString(QString());
            setResult(result);
            setStatus(Ready);
        }
        
        emit finished();
    }
    else {
        listStreams(channel.value("id").toString(), "rtsp");
    }    
}

QString TVRequest::getRedirect(const QNetworkReply *reply) {
    QString redirect = QString::fromUtf8(reply->rawHeader("Location"));
    
    if (redirect.startsWith("/")) {
        redirect.prepend(reply->url().scheme() + "://" + reply->url().authority());
    }
    
    return redirect;
}

void TVRequest::followRedirect(const QString &url, const char *slot) {
    m_redirects++;
    QNetworkReply *reply = networkAccessManager()->get(QNetworkRequest(url));
    connect(reply, SIGNAL(finished()), this, slot);
    connect(this, SIGNAL(finished()), reply, SLOT(deleteLater()));
}

QNetworkAccessManager* TVRequest::networkAccessManager() {
    return m_nam ? m_nam : m_nam = new QNetworkAccessManager(this);
}
