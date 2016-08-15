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

#include "youtube.h"
#include "database.h"
#include "json.h"
#include "logger.h"
#include <qyoutube/urls.h>
#include <QSettings>
#include <QSqlRecord>
#if QT_VERSION >= 0x050000
#include <QUrlQuery>
#endif

const QString YouTube::API_KEY("AIzaSyDhIlkLzHJKDCNr6thsjlQpZrkY3lO_Uu4");
const QString YouTube::CLIENT_ID("957843447749-ur7hg6de229ug0svjakaiovok76s6ecr.apps.googleusercontent.com");
const QString YouTube::CLIENT_SECRET("dDs2_WwgS16LZVuzqA9rIg-I");
const QStringList YouTube::SCOPES = QStringList() << QYouTube::READ_WRITE_SCOPE << QYouTube::FORCE_SSL_SCOPE;

const QRegExp YouTube::DURATION_REGEXP("T((\\d+)H|)((\\d+)M|)((\\d+)S|)");
const QRegExp YouTube::URL_REGEXP("(http(s|)://(www.|m.|)youtube.com/(v/|.+)(v=|list=|)|http://youtu.be/)",
                                  Qt::CaseInsensitive);

YouTube::SubscriptionCache YouTube::subscriptionCache;

YouTube* YouTube::self = 0;

YouTube::YouTube() :
    QObject()
{
}

YouTube::~YouTube() {
    self = 0;
}

YouTube* YouTube::instance() {
    return self ? self : self = new YouTube;
}

void YouTube::init() {
    if (accessToken().isEmpty()) {
        setUserId(QString());
    }
}

QString YouTube::formatDuration(const QString &duration) {
    if (DURATION_REGEXP.indexIn(duration) != -1) {        
        const int hours = qMax(0, DURATION_REGEXP.cap(2).toInt());
        const int mins = qMax(0, DURATION_REGEXP.cap(4).toInt());
        const int secs = qMax(0, DURATION_REGEXP.cap(6).toInt());
        
        if (hours > 0) {
            return QString("%1:%2:%3").arg(hours, 2, 10, QChar('0'))
                                      .arg(mins, 2, 10, QChar('0'))
                                      .arg(secs, 2, 10, QChar('0'));
        }
        
        return QString("%1:%2").arg(mins, 2, 10, QChar('0')).arg(secs, 2, 10, QChar('0'));
    }
    
    return QString("00:00");
}

QString YouTube::getErrorString(const QVariantMap &error) {
    if (error.contains("error_description")) {
        return error.value("error_description").toString();
    }

    if (error.contains("error")) {
        const QVariant e = error.value("error");

        if (e.type() == QVariant::String) {
            return e.toString();
        }

        const QVariantMap map = e.toMap();

        if (map.contains("message")) {
            return map.value("message").toString();
        }

        return tr("Unknown error");
    }
    
    if (error.contains("errors")) {
        foreach (const QVariant &e, error.value("errors").toList()) {
            const QVariantMap m = e.toMap();
            
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

QString YouTube::relatedPlaylist(const QString &name) {
    return relatedPlaylists().value(name).toString();
}

QUrl YouTube::authUrl() {
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

QString YouTube::userId() {
    return QSettings().value("YouTube/userId").toString();
}

void YouTube::setUserId(const QString &id) {
    Logger::log("YouTube::setUserId(). ID: " + id, Logger::MediumVerbosity);
    
    if (id != userId()) {
        QSettings().setValue("YouTube/userId", id);
        subscriptionCache.ids.clear();
        subscriptionCache.nextPageToken = QString();
        subscriptionCache.loaded = false;

        if (self) {
            emit self->userIdChanged(id);
        }
    }
}

QString YouTube::accessToken() {
    if (userId().isEmpty()) {
        return QString();
    }
    
    QSqlQuery query = getDatabase().exec(QString("SELECT accessToken FROM youtubeAccounts WHERE userId = '%1'")
                                                .arg(userId()));
    
    if (query.lastError().isValid()) {
        Logger::log("YouTube::accessToken(): database error: " + query.lastError().text());
        return QString();
    }
    
    if ((query.record().count() > 0) && (query.next())) {
         return query.value(0).toString();
    }
    
    return QString();
}

void YouTube::setAccessToken(const QString &token) {
    Logger::log("YouTube::setAccessToken(). Token: " + token, Logger::MediumVerbosity);
    QSqlQuery query = getDatabase().exec(QString("UPDATE youtubeAccounts SET accessToken = '%1' WHERE userId = '%2'")
                                                .arg(token).arg(userId()));

    if (query.lastError().isValid()) {
        Logger::log("YouTube::setAccessToken(): database error: " + query.lastError().text());
    }
    else if (self) {
        emit self->accessTokenChanged(token);
    }
}

QString YouTube::refreshToken() {
    if (userId().isEmpty()) {
        return QString();
    }
    
    QSqlQuery query = getDatabase().exec(QString("SELECT refreshToken FROM youtubeAccounts WHERE userId = '%1'")
                                                .arg(userId()));
    
    if (query.lastError().isValid()) {
        Logger::log("YouTube::refreshToken(): database error: " + query.lastError().text());
        return QString();
    }
    
    if ((query.record().count() > 0) && (query.next())) {
         return query.value(0).toString();
    }
    
    return QString();
}

void YouTube::setRefreshToken(const QString &token) {
    Logger::log("YouTube::setRefreshToken(). Token: " + token, Logger::MediumVerbosity);
    QSqlQuery query = getDatabase().exec(QString("UPDATE youtubeAccounts SET refreshToken = '%1' WHERE userId = '%2'")
                                                .arg(token).arg(userId()));

    if (query.lastError().isValid()) {
        Logger::log("YouTube::setRefreshToken(): database error: " + query.lastError().text());
    }
    else if (self) {
        emit self->refreshTokenChanged(token);
    }
}

QVariantMap YouTube::relatedPlaylists() {
    if (userId().isEmpty()) {
        return QVariantMap();
    }
    
    QSqlQuery query = getDatabase().exec(QString("SELECT relatedPlaylists FROM youtubeAccounts WHERE userId = '%1'")
                                                .arg(userId()));
    
    if (query.lastError().isValid()) {
        Logger::log("YouTube::refreshToken(): database error: " + query.lastError().text());
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
        Logger::log("YouTube::setRelatedPlaylists(): database error: " + query.lastError().text());
    }
    else if (self) {
        emit self->relatedPlaylistsChanged(playlists);
    }
}

QString YouTube::apiKey() {
    return QSettings().value("YouTube/apiKey", API_KEY).toString();
}

void YouTube::setApiKey(const QString &key) {
    Logger::log("YouTube::setApiKey(). Key: " + key, Logger::MediumVerbosity);
    
    if (key != apiKey()) {
        QSettings().setValue("YouTube/apiKey", key);

        if (self) {
            emit self->apiKeyChanged(key);
        }
    }
}

QString YouTube::clientId() {
    return QSettings().value("YouTube/clientId", CLIENT_ID).toString();
}

void YouTube::setClientId(const QString &id) {
    Logger::log("YouTube::setClientId(). ID: " + id, Logger::MediumVerbosity);
    
    if (id != clientId()) {
        QSettings().setValue("YouTube/clientId", id);

        if (self) {
            emit self->clientIdChanged(id);
        }
    }
}

QString YouTube::clientSecret() {
    return QSettings().value("YouTube/clientSecret", CLIENT_SECRET).toString();
}

void YouTube::setClientSecret(const QString &secret) {
    Logger::log("YouTube::setClientSecret(). Secret: " + secret, Logger::MediumVerbosity);
    
    if (secret != clientSecret()) {
        QSettings().setValue("YouTube/clientSecret", secret);

        if (self) {
            emit self->clientSecretChanged(secret);
        }
    }
}

QStringList YouTube::scopes() {
    return QSettings().value("YouTube/scopes", SCOPES).toStringList();
}

void YouTube::setScopes(const QStringList &s) {
    Logger::log("YouTube::setScopes(). Scopes: " + s.join(", "), Logger::MediumVerbosity);
    
    if (s != scopes()) {
        QSettings().setValue("YouTube/scopes", s);

        if (self) {
            emit self->scopesChanged(s);
        }
    }
}

bool YouTube::hasScope(const QString &scope) {
    if (userId().isEmpty()) {
        return false;
    }
    
    QSqlQuery query = getDatabase().exec(QString("SELECT scopes FROM youtubeAccounts WHERE userId = '%1'")
                                                .arg(userId()));
    
    if (query.lastError().isValid()) {
        Logger::log("YouTube::hasScope(): database error: " + query.lastError().text());
        return false;
    }
    
    if ((query.record().count() > 0) && (query.next())) {
         return query.value(0).toString().contains(scope);
    }
    
    return false;
}

QString YouTube::auditScope() {
    return QYouTube::AUDIT_SCOPE;
}

QString YouTube::forceSslScope() {
    return QYouTube::FORCE_SSL_SCOPE;
}

QString YouTube::partnerScope() {
    return QYouTube::PARTNER_SCOPE;
}

QString YouTube::readOnlyScope() {
    return QYouTube::READ_ONLY_SCOPE;
}

QString YouTube::readWriteScope() {
    return QYouTube::READ_WRITE_SCOPE;
}

QString YouTube::uploadScope() {
    return QYouTube::UPLOAD_SCOPE;
}
