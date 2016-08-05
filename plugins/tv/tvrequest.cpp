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
#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>
#include <QSqlRecord>
#ifdef Q_OS_SYMBIAN
#include <QCoreApplication>
#include <QDebug>
#include <QFile>
#endif
#if QT_VERSION >= 0x050000
#include <QUrlQuery>
#endif

const QString TVRequest::API_URL("http://www.filmon.tv/api-v2");
const QString TVRequest::CHANNEL_URL(API_URL + "/channel");
const QString TVRequest::LOGO_URL("http://static.filmon.com/assets/channels/%1/big_logo.png");
#ifdef Q_OS_LINUX
const QString TVRequest::DATABASE_NAME("/opt/cutetube2/plugins/cutetube2-tv/cutetube2-tv.db");
#else
const QString TVRequest::DATABASE_NAME("cutetube2-tv.db");
#endif
const QRegExp TVRequest::DATABASE_LIMIT("LIMIT (\\d+), (\\d+)");

const int TVRequest::MAX_REDIRECTS = 8;

bool TVRequest::dbinit = false;

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

bool TVRequest::get(const QString &resourceType, const QString &resourceId) {
    if ((status() == Loading) || (resourceId.isEmpty())) {
        return false;
    }

    if (resourceType == "video") {
        getChannel(resourceId);
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
        listGroups(resourceId);
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

void TVRequest::getChannel(const QString &id) {
    QSqlQuery query = getDatabase().exec("SELECT * FROM channels WHERE id = " + getChannelId(id));
    
    if ((query.record().count() > 0) && (query.next())) {
        setErrorString(QString());
        setResult(getChannel(query));
        setStatus(Ready);
    }
    else {
        const QSqlError error = query.lastError();
        
        if (error.isValid()) {
            setErrorString(error.text());
        }
        else {
            setErrorString(tr("Database error"));
        }
        
        setResult(QVariant());
        setStatus(Failed);
    }
    
    emit finished();
}

void TVRequest::listChannels(const QString &idOrQuery) {
    if (idOrQuery.contains("group_")) {
        listChannelsByGroup(idOrQuery.section("group_", -1));
    }
    else {
        listChannelsByQuery(idOrQuery);
    }
}

void TVRequest::listChannelsByGroup(const QString &id) {
    listChannels(QString("WHERE groupId = %1 ORDER BY title ASC LIMIT 0, 20").arg(id));
}

void TVRequest::listChannelsByQuery(const QString &queryString) {
    QSqlQuery query = getDatabase().exec("SELECT * FROM channels " + queryString);
    const QSqlError error = query.lastError();
    
    if (error.isValid()) {
        setErrorString(error.text());
        setResult(QVariant());
        setStatus(Failed);
    }
    else {
        QVariantMap result;
        QVariantList items;
        
        while (query.next()) {
            items << getChannel(query);
        }
        
        result["items"] = items;
        
        if (!items.isEmpty()) {
            const int pos = DATABASE_LIMIT.indexIn(queryString);
            
            if (pos != -1) {
                const int offset = qMax(0, DATABASE_LIMIT.cap(1).toInt()) + items.size();
                const int max = qMax(1, DATABASE_LIMIT.cap(2).toInt());
                result["next"] = QString("%1 LIMIT %2, %3").arg(queryString.left(pos)).arg(offset).arg(max);
            }
        }
        
        setErrorString(QString());
        setResult(result);
        setStatus(Ready);
    }
    
    emit finished();
}

void TVRequest::searchChannels(const QString &query, const QString &order) {
    listChannelsByQuery(QString("WHERE title LIKE '%%1%' OR description LIKE '%%1%' %2").arg(query).arg(order));
}

void TVRequest::listGroups(const QString &queryString) {
    QSqlQuery query = getDatabase().exec("SELECT * FROM groups " + queryString);
    const QSqlError error = query.lastError();
    
    if (error.isValid()) {
        setErrorString(error.text());
        setResult(QVariant());
        setStatus(Failed);
    }
    else {
        QVariantMap result;
        QVariantList items;
        
        while (query.next()) {
            items << getGroup(query);
        }
        
        result["items"] = items;
        setErrorString(QString());
        setResult(result);
        setStatus(Ready);
    }
    
    emit finished();
}

void TVRequest::listStreams(const QString &id, const QString &protocol) {
    setStatus(Loading);
    m_redirects = 0;
    QNetworkReply *reply =
    networkAccessManager()->get(QNetworkRequest(QString("%1/%2?protocol=%3").arg(CHANNEL_URL)
                                                                            .arg(getChannelId(id))
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

void TVRequest::initDatabase() {
#ifdef Q_OS_SYMBIAN
    const QString name(QCoreApplication::applicationDirPath() + "/cutetube2-tv.db");
    QString oldName("C:/cutetube2/plugins/cutetube2-tv.db");

    if (!QFile::exists(oldName)) {
        oldName.replace(0, 1, "E");
    }

    if (!QFile::exists(oldName)) {
        oldName.replace(0, 1, "F");
    }

    if (QFile::exists(oldName)) {
        if (QFile::exists(name)) {
            qDebug() << "TVRequest::initDatabase(): Removing old database" << name << "Result:" << QFile::remove(name);
        }

        qDebug() << "TVRequest::initDatabase(): Moving database from " << oldName << "to" << name
                 << "Result:" << QFile::rename(oldName, name);
    }
#endif
    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE", "cutetube2-tv");
    db.setDatabaseName(DATABASE_NAME);
    dbinit = db.isValid();
}

QSqlDatabase TVRequest::getDatabase() {
    if (!dbinit) {
        initDatabase();
    }
    
    return QSqlDatabase::database("cutetube2-tv", true);
}

QVariantMap TVRequest::getChannel(const QSqlQuery &query) {
    QVariantMap channel;
    const QString id = query.value(0).toString();
    const QString logo = LOGO_URL.arg(id);
    channel["id"] = QString("channel_%1_group_%2").arg(id).arg(query.value(3).toString());
    channel["title"] = query.value(1);
    channel["description"] = query.value(2);
    channel["largeThumbnailUrl"] = logo;
    channel["thumbnailUrl"] = logo;
    channel["url"] = QString("%1/%2").arg(CHANNEL_URL).arg(id);
    channel["downloadable"] = false;
    return channel;
}

QString TVRequest::getChannelId(const QString &id) {
    return id.section("channel_", -1).section("_", 0, 0);
}

QVariantMap TVRequest::getGroup(const QSqlQuery &query) {
    QVariantMap group;
    group["id"] = "group_" + query.value(0).toString();
    group["title"] = query.value(1);
    return group;
}

QString TVRequest::getGroupId(const QString &id) {
    return id.section("group_", -1).section("_", 0, 0);
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
