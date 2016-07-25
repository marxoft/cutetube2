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

#include "dailymotion.h"
#include "database.h"
#include "logger.h"
#include <qdailymotion/urls.h>
#include <QSettings>
#include <QSqlRecord>
#if QT_VERSION >= 0x050000
#include <QUrlQuery>
#endif

const QString Dailymotion::CLIENT_ID("71e91852b1af82bfcb41");
const QString Dailymotion::CLIENT_SECRET("eed72ef8e1ef42d0c72fa3b1c7905a9e1abc079e");
const QString Dailymotion::REDIRECT_URI("http://marxoft.co.uk/projects/cutetube");
const QStringList Dailymotion::SCOPES = QStringList() << QDailymotion::USER_INFO_SCOPE
                                                      << QDailymotion::MANAGE_VIDEOS_SCOPE
                                                      << QDailymotion::MANAGE_COMMENTS_SCOPE
                                                      << QDailymotion::MANAGE_PLAYLISTS_SCOPE
                                                      << QDailymotion::MANAGE_SUBSCRIPTIONS_SCOPE
                                                      << QDailymotion::MANAGE_FAVORITES_SCOPE
                                                      << QDailymotion::MANAGE_GROUPS_SCOPE;

const QStringList Dailymotion::CATEGORY_FIELDS = QStringList() << "id" << "name";

const QStringList Dailymotion::COMMENT_FIELDS = QStringList() << "id" << "created_time" << "message" << "owner.id"
                                                              << "owner.screenname" << "owner.avatar_60_url"
                                                              << "video.id";
                                                
const QStringList Dailymotion::PLAYLIST_FIELDS = QStringList() << "id" << "created_time" << "description"
                                                               << "name" << "owner.id" << "owner.screenname"
                                                               << "thumbnail_120_url" << "thumbnail_url"
                                                               << "videos_total";

const QStringList Dailymotion::USER_FIELDS = QStringList() << "id" << "avatar_60_url" << "avatar_240_url"
                                                           << "cover_150_url" << "cover_url" << "description"
                                                           << "fans_total" << "screenname" << "views_total";

const QStringList Dailymotion::VIDEO_FIELDS = QStringList() << "id" << "created_time" << "description"
                                                            << "duration" << "favorited_at" << "owner.id"
                                                            << "owner.screenname" << "thumbnail_120_url"
                                                            << "thumbnail_url" << "title" << "url" << "views_total";
                                                
const QRegExp Dailymotion::URL_REGEXP("(http(s|)://(www.|)dailymotion.com/|http://dai.ly/)\\w+", Qt::CaseInsensitive);

Dailymotion::SubscriptionCache Dailymotion::subscriptionCache;

Dailymotion* Dailymotion::self = 0;

Dailymotion::Dailymotion() :
    QObject()
{
}

Dailymotion::~Dailymotion() {
    self = 0;
}

Dailymotion* Dailymotion::instance() {
    return self ? self : self = new Dailymotion;
}

void Dailymotion::init() {
    if (accessToken().isEmpty()) {
        setUserId(QString());
    }
}

QString Dailymotion::getErrorString(const QVariantMap &error) {
    const QVariantMap em = error.contains("error") ? error.value("error").toMap() : error;

    if (em.contains("message")) {
        return em.value("message").toString();
    }
    
    return tr("Unknown error");
}

QUrl Dailymotion::authUrl() {
    QUrl url(QDailymotion::AUTH_URL);
#if QT_VERSION >= 0x050000
    QUrlQuery query(url);
    query.addQueryItem("client_id", clientId());
    query.addQueryItem("redirect_uri", redirectUri());
    query.addQueryItem("response_type", "code");
    query.addQueryItem("display", "popup");
    
    QStringList s = scopes();
    
    if (!s.isEmpty()) {
        query.addQueryItem("scope", s.join("+"));
    }
    
    url.setQuery(query);
#else
    url.addQueryItem("client_id", clientId());
    url.addQueryItem("redirect_uri", redirectUri());
    url.addQueryItem("response_type", "code");
    url.addQueryItem("display", "popup");
    
    QStringList s = scopes();
    
    if (!s.isEmpty()) {
        url.addQueryItem("scope", s.join("+"));
    }
#endif
    return url;
}

QString Dailymotion::userId() {
    return QSettings().value("Dailymotion/userId").toString();
}

void Dailymotion::setUserId(const QString &id) {
    if (id != userId()) {
        QSettings().setValue("Dailymotion/userId", id);
        subscriptionCache.ids.clear();
        subscriptionCache.filters.clear();
        subscriptionCache.hasMore = true;

        if (self) {
            emit self->userIdChanged(id);
        }
    }
}

QString Dailymotion::accessToken() {
    if (userId().isEmpty()) {
        return QString();
    }
    
    QSqlQuery query = getDatabase().exec(QString("SELECT accessToken FROM dailymotionAccounts WHERE userId = '%1'")
                                                .arg(userId()));
    
    if (query.lastError().isValid()) {
        Logger::log("Dailymotion::accessToken(): database error: " + query.lastError().text());
        return QString();
    }
    
    if ((query.record().count() > 0) && (query.next())) {
         return query.value(0).toString();
    }
    
    return QString();
}

void Dailymotion::setAccessToken(const QString &token) {
    QSqlQuery query = getDatabase().exec(QString("UPDATE dailymotionAccounts SET accessToken = '%1' WHERE userId = '%2'")
                                                .arg(token).arg(userId()));

    if (query.lastError().isValid()) {
        Logger::log("Dailymotion::setAccessToken(): database error: " + query.lastError().text());
    }
    else if (self) {
        emit self->accessTokenChanged(token);
    }
}

QString Dailymotion::refreshToken() {
    if (userId().isEmpty()) {
        return QString();
    }
    
    QSqlQuery query = getDatabase().exec(QString("SELECT refreshToken FROM dailymotionAccounts WHERE userId = '%1'")
                                                .arg(userId()));
    
    if (query.lastError().isValid()) {
        Logger::log("Dailymotion::refreshToken(): database error: " + query.lastError().text());
        return QString();
    }
    
    if ((query.record().count() > 0) && (query.next())) {
         return query.value(0).toString();
    }
    
    return QString();
}

void Dailymotion::setRefreshToken(const QString &token) {
    QSqlQuery query = getDatabase().exec(QString("UPDATE dailymotionAccounts SET refreshToken = '%1' WHERE userId = '%2'")
                                                .arg(token).arg(userId()));

    if (query.lastError().isValid()) {
        Logger::log("Dailymotion::setRefreshToken(): database error: " + query.lastError().text());
    }
    else if (self) {
        emit self->accessTokenChanged(token);
    }
}

QString Dailymotion::clientId() {
    return QSettings().value("Dailymotion/clientId", CLIENT_ID).toString();
}

void Dailymotion::setClientId(const QString &id) {
    if (id != clientId()) {
        QSettings().setValue("Dailymotion/clientId", id);

        if (self) {
            emit self->clientIdChanged(id);
        }
    }
}

QString Dailymotion::clientSecret() {
    return QSettings().value("Dailymotion/clientSecret", CLIENT_SECRET).toString();
}

void Dailymotion::setClientSecret(const QString &secret) {
    if (secret != clientSecret()) {
        QSettings().setValue("Dailymotion/clientSecret", secret);

        if (self) {
            emit self->clientSecretChanged(secret);
        }
    }
}

QString Dailymotion::redirectUri() {
    return QSettings().value("Dailymotion/redirectUri", REDIRECT_URI).toString();
}

void Dailymotion::setRedirectUri(const QString &uri) {
    if (uri != redirectUri()) {
        QSettings().setValue("Dailymotion/redirectUri", uri);

        if (self) {
            emit self->redirectUriChanged(uri);
        }
    }
}

QStringList Dailymotion::scopes() {
    return QSettings().value("Dailymotion/scopes", SCOPES).toStringList();
}

void Dailymotion::setScopes(const QStringList &s) {
    if (s != scopes()) {
        QSettings().setValue("Dailymotion/scopes", s);

        if (self) {
            emit self->scopesChanged(s);
        }
    }
}

bool Dailymotion::hasScope(const QString &scope) {
    if (userId().isEmpty()) {
        return false;
    }
    
    QSqlQuery query = getDatabase().exec(QString("SELECT scopes FROM dailymotionAccounts WHERE userId = '%1'")
                                                .arg(userId()));
    
    if (query.lastError().isValid()) {
        Logger::log("Dailymotion::hasScope(): database error: " + query.lastError().text());
        return false;
    }
    
    if ((query.record().count() > 0) && (query.next())) {
         return query.value(0).toString().contains(scope);
    }
    
    return false;
}

QString Dailymotion::emailScope() {
    return QDailymotion::EMAIL_SCOPE;
}

QString Dailymotion::manageCommentsScope() {
    return QDailymotion::MANAGE_COMMENTS_SCOPE;
}

QString Dailymotion::manageFavouritesScope() {
    return QDailymotion::MANAGE_FAVORITES_SCOPE;
}

QString Dailymotion::manageFriendsScope() {
    return QDailymotion::MANAGE_FRIENDS_SCOPE;
}

QString Dailymotion::manageGroupsScope() {
    return QDailymotion::MANAGE_GROUPS_SCOPE;
}

QString Dailymotion::managePlaylistsScope() {
    return QDailymotion::MANAGE_PLAYLISTS_SCOPE;
}

QString Dailymotion::manageSubscriptionsScope() {
    return QDailymotion::MANAGE_SUBSCRIPTIONS_SCOPE;
}

QString Dailymotion::manageTilesScope() {
    return QDailymotion::MANAGE_TILES_SCOPE;
}

QString Dailymotion::manageVideosScope() {
    return QDailymotion::MANAGE_VIDEOS_SCOPE;
}

QString Dailymotion::userInfoScope() {
    return QDailymotion::USER_INFO_SCOPE;
}
