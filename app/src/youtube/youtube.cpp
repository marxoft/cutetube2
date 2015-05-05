/*
 * Copyright (C) 2015 Stuart Howarth <showarth@marxoft.co.uk>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License version 3 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "youtube.h"
#include "database.h"
#include "json.h"
#include <qyoutube/urls.h>
#include <QSettings>
#include <QSqlRecord>
#if QT_VERSION >= 0x050000
#include <QUrlQuery>
#endif
#ifdef CUTETUBE_DEBUG
#include <QDebug>
#endif

static const QString API_KEY("AIzaSyDhIlkLzHJKDCNr6thsjlQpZrkY3lO_Uu4");
static const QString CLIENT_ID("957843447749-ur7hg6de229ug0svjakaiovok76s6ecr.apps.googleusercontent.com");
static const QString CLIENT_SECRET("dDs2_WwgS16LZVuzqA9rIg-I");
static const QStringList SCOPES = QStringList() << QYouTube::READ_WRITE_SCOPE << QYouTube::FORCE_SSL_SCOPE;

const QRegExp YouTube::URL_REGEXP("(http(s|)://(www.|m.|)youtube.com/(v/|.+)(v=|list=|)|http://youtu.be/)",
                                  Qt::CaseInsensitive);

YouTube::SubscriptionCache YouTube::subscriptionCache;

YouTube* YouTube::self = 0;

YouTube::YouTube(QObject *parent) :
    QObject(parent)
{
    if (!self) {
        self = this;
    }
}

YouTube::~YouTube() {
    if (self == this) {
        self = 0;
    }
}

YouTube* YouTube::instance() {
    return self;
}

QString YouTube::formatDuration(const QString &duration) {    
    QStringList nums;
    QString num;
    QRegExp re;
    
    foreach (QString s, QStringList() << "H" << "M" << "S") {
        if (duration.contains(s)) {
            re.setPattern("\\d+(?=" + s + ")");
        
            if (re.indexIn(duration) != -1) {
                num = re.cap();
            
                if (num.size() < 2) {
                    num.prepend("0");
                }
                
                nums << num;
            }
            else if (s != "H") {
                nums << "00";
            }
        }
        else if (s != "H") {
            nums << "00";
        }
    }

    return nums.join(":");
}

QString YouTube::getErrorString(const QVariantMap &error) {
    QVariantMap em = error.contains("error") ? error.value("error").toMap() : error;

    if (em.contains("message")) {
        return em.value("message").toString();
    }
    
    if (em.contains("errors")) {
        foreach (QVariant e, error.value("errors").toList()) {
            QVariantMap m = e.toMap();
            
            if (m.contains("message")) {
                return m.value("message").toString();
            }
        }
    }
    
    return tr("Unknown error");
}

QString YouTube::getPlaylistId(const QVariantMap &playlist) {
    if (playlist.value("kind") == "youtube#searchResult") {
        return playlist.value("id").toMap().value("playlistId").toString();
    }
    
    return playlist.value("id").toString();
}

QString YouTube::getUserId(const QVariantMap &user) {
    if (user.value("kind") == "youtube#searchResult") {
        return user.value("id").toMap().value("channelId").toString();
    }
    
    if (user.value("kind") == "youtube#subscription") {
        return user.value("snippet").toMap().value("resourceId").toMap().value("channelId").toString();
    }
    
    return user.value("id").toString();
}

QString YouTube::getVideoId(const QVariantMap &video) {
    if (video.value("kind") == "youtube#searchResult") {
        return video.value("id").toMap().value("videoId").toString();
    }
    
    if (video.value("kind") == "youtube#playlistItem") {
        return video.value("snippet").toMap().value("resourceId").toMap().value("videoId").toString();
    }
    
    if (video.value("kind") == "youtube#activity") {
        QString type = video.value("snippet").toMap().value("type").toString();
        
        if (type == "upload") {
            return video.value("contentDetails").toMap().value("upload").toMap().value("videoId").toString();
        }
        else {
            return video.value("contentDetails").toMap().value(type).toMap().value("resourceId").toMap()
                   .value("videoId").toString();
        }
    }
    
    return video.value("id").toString();
}

QString YouTube::relatedPlaylist(const QString &name) const {
    return relatedPlaylists().value(name).toString();
}

QUrl YouTube::authUrl() const {
    QUrl url(QYouTube::AUTH_URL);
#if QT_VERSION >= 0x050000
    QUrlQuery query(url);
    query.addQueryItem("client_id", clientId());
    query.addQueryItem("redirect_uri", QYouTube::REDIRECT_URI);
    query.addQueryItem("response_type", "code");
    query.addQueryItem("access_type", "offline");
    query.addQueryItem("display", "popup");
    
    QStringList s = scopes();
    
    if (!s.isEmpty()) {
        query.addQueryItem("scope", s.join(" "));
    }
    
    url.setQuery(query);
#else
    url.addQueryItem("client_id", clientId());
    url.addQueryItem("redirect_uri", QYouTube::REDIRECT_URI);
    url.addQueryItem("response_type", "code");
    url.addQueryItem("access_type", "offline");
    url.addQueryItem("display", "popup");
    
    QStringList s = scopes();
    
    if (!s.isEmpty()) {
        url.addQueryItem("scope", s.join(" "));
    }
#endif
    return url;
}

QString YouTube::userId() const {
    return QSettings().value("YouTube/userId").toString();
}

void YouTube::setUserId(const QString &id) {
    if (id != userId()) {
        QSettings().setValue("YouTube/userId", id);
        subscriptionCache.ids.clear();
        subscriptionCache.nextPageToken = QString();
        emit userIdChanged();
    }
}

QString YouTube::accessToken() const {
    if (userId().isEmpty()) {
        return QString();
    }
    
    QSqlQuery query = getDatabase().exec(QString("SELECT accessToken FROM youtubeAccounts WHERE userId = '%1'")
                                                .arg(userId()));
    
    if (query.lastError().isValid()) {
        qDebug() << "YouTube::accessToken: database error:" << query.lastError().text();
        return QString();
    }
    
    if ((query.record().count() > 0) && (query.next())) {
         return query.value(0).toString();
    }
    
    return QString();
}

void YouTube::setAccessToken(const QString &token) {
    QSqlQuery query = getDatabase().exec(QString("UPDATE youtubeAccounts SET accessToken = '%1' WHERE userId = '%2'")
                                                .arg(token).arg(userId()));

    if (query.lastError().isValid()) {
        qDebug() << "YouTube::setAccessToken: database error:" << query.lastError().text();
    }
    else {
        emit accessTokenChanged();
    }
#ifdef CUTETUBE_DEBUG
    qDebug() << "YouTube::setAccessToken" << token;
#endif
}

QString YouTube::refreshToken() const {
    if (userId().isEmpty()) {
        return QString();
    }
    
    QSqlQuery query = getDatabase().exec(QString("SELECT refreshToken FROM youtubeAccounts WHERE userId = '%1'")
                                                .arg(userId()));
    
    if (query.lastError().isValid()) {
        qDebug() << "YouTube::refreshToken: database error:" << query.lastError().text();
        return QString();
    }
    
    if ((query.record().count() > 0) && (query.next())) {
         return query.value(0).toString();
    }
    
    return QString();
}

void YouTube::setRefreshToken(const QString &token) {
    QSqlQuery query = getDatabase().exec(QString("UPDATE youtubeAccounts SET refreshToken = '%1' WHERE userId = '%2'")
                                                .arg(token).arg(userId()));

    if (query.lastError().isValid()) {
        qDebug() << "YouTube::setRefreshToken: database error:" << query.lastError().text();
    }
    else {
        emit refreshTokenChanged();
    }
#ifdef CUTETUBE_DEBUG
    qDebug() << "YouTube::setRefreshToken" << token;
#endif
}

QVariantMap YouTube::relatedPlaylists() const {
    if (userId().isEmpty()) {
        return QVariantMap();
    }
    
    QSqlQuery query = getDatabase().exec(QString("SELECT relatedPlaylists FROM youtubeAccounts WHERE userId = '%1'")
                                                .arg(userId()));
    
    if (query.lastError().isValid()) {
        qDebug() << "YouTube::refreshToken: database error:" << query.lastError().text();
        return QVariantMap();
    }
    
    if ((query.record().count() > 0) && (query.next())) {
         return QtJson::Json::parse(query.value(0).toString()).toMap();
    }
    
    return QVariantMap();
}

void YouTube::setRelatedPlaylists(const QVariantMap &playlists) {
    QSqlQuery query = getDatabase().exec(QString("UPDATE youtubeAccounts SET relatedPlaylists = '%1' WHERE userId = '%2'")
                                                .arg(QString(QtJson::Json::serialize(playlists))).arg(userId()));

    if (query.lastError().isValid()) {
        qDebug() << "YouTube::setRelatedPlaylists: database error:" << query.lastError().text();
    }
    else {
        emit relatedPlaylistsChanged();
    }
#ifdef CUTETUBE_DEBUG
    qDebug() << "YouTube::setRelatedPlaylists" << playlists;
#endif
}

QString YouTube::apiKey() const {
    return QSettings().value("YouTube/apiKey", API_KEY).toString();
}

void YouTube::setApiKey(const QString &key) {
    if (key != apiKey()) {
        QSettings().setValue("YouTube/apiKey", key);
        emit apiKeyChanged();
    }
#ifdef CUTETUBE_DEBUG
    qDebug() << "YouTube::setApiKey" << key;
#endif
}

QString YouTube::clientId() const {
    return QSettings().value("YouTube/clientId", CLIENT_ID).toString();
}

void YouTube::setClientId(const QString &id) {
    if (id != clientId()) {
        QSettings().setValue("YouTube/clientId", id);
        emit clientIdChanged();
    }
#ifdef CUTETUBE_DEBUG
    qDebug() << "YouTube::setClientId" << id;
#endif
}

QString YouTube::clientSecret() const {
    return QSettings().value("YouTube/clientSecret", CLIENT_SECRET).toString();
}

void YouTube::setClientSecret(const QString &secret) {
    if (secret != clientSecret()) {
        QSettings().setValue("YouTube/clientSecret", secret);
        emit clientSecretChanged();
    }
#ifdef CUTETUBE_DEBUG
    qDebug() << "YouTube::setClientSecret" << secret;
#endif
}

QStringList YouTube::scopes() const {
    return QSettings().value("YouTube/scopes", SCOPES).toStringList();
}

void YouTube::setScopes(const QStringList &s) {
    if (s != scopes()) {
        QSettings().setValue("YouTube/scopes", s);
        emit scopesChanged();
    }
#ifdef CUTETUBE_DEBUG
    qDebug() << "YouTube::setScopes" << s;
#endif
}

bool YouTube::hasScope(const QString &scope) const {
    if (userId().isEmpty()) {
        return false;
    }
    
    QSqlQuery query = getDatabase().exec(QString("SELECT scopes FROM youtubeAccounts WHERE userId = '%1'")
                                                .arg(userId()));
    
    if (query.lastError().isValid()) {
        qDebug() << "YouTube::hasScope: database error:" << query.lastError().text();
        return false;
    }
    
    if ((query.record().count() > 0) && (query.next())) {
         return query.value(0).toString().contains(scope);
    }
    
    return false;
}
