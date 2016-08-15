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

#include "vimeo.h"
#include "database.h"
#include "logger.h"
#include <qvimeo/urls.h>
#include <QSettings>
#include <QSqlRecord>
#if QT_VERSION >= 0x050000
#include <QUrlQuery>
#endif

const QString Vimeo::CLIENT_ID("0bf284bf5a0e46630f5097a590a76ef976a94322");
const QString Vimeo::CLIENT_SECRET("7nnZ1OPS13hjKAVhzuXx/4AIdKxmgDNasHkj5QraWWkrNsd6mxYWZG73AKaFUdLzoNWhGA75jSffs+JyAFfi\
0MiFi1OXnzHsxaL0HCIFpxk0GpZlXcScWmJTHvGGtVv1");
const QString Vimeo::CLIENT_TOKEN("fb5c66ebe6938e858921963f850355a0");
const QString Vimeo::REDIRECT_URI("http://marxoft.co.uk/projects/cutetube");
const QStringList Vimeo::SCOPES = QStringList() << QVimeo::PUBLIC_SCOPE << QVimeo::PRIVATE_SCOPE << QVimeo::CREATE_SCOPE
                                                << QVimeo::EDIT_SCOPE << QVimeo::DELETE_SCOPE << QVimeo::INTERACT_SCOPE;

const QRegExp Vimeo::URL_REGEXP("http(s|)://vimeo.com/\\w+", Qt::CaseInsensitive);

Vimeo::SubscriptionCache Vimeo::subscriptionCache;

Vimeo* Vimeo::self = 0;

Vimeo::Vimeo() :
    QObject()
{
}

Vimeo::~Vimeo() {
    self = 0;
}

Vimeo* Vimeo::instance() {
    return self ? self : self = new Vimeo;
}

void Vimeo::init() {
    if (accessToken().isEmpty()) {
        setUserId(QString());
    }
}

QString Vimeo::getErrorString(const QVariantMap &error) {
    return error.contains("error") ? error.value("error").toString() : tr("Unknown error");
}

QUrl Vimeo::authUrl() {
    QUrl url(QVimeo::AUTH_URL);
#if QT_VERSION >= 0x050000
    QUrlQuery query(url);
    query.addQueryItem("client_id", clientId());
    query.addQueryItem("redirect_uri", redirectUri());
    query.addQueryItem("response_type", "code");
    
    QStringList s = scopes();
    
    if (!s.isEmpty()) {
        query.addQueryItem("scope", s.join("+"));
    }
    
    url.setQuery(query);
#else
    url.addQueryItem("client_id", clientId());
    url.addQueryItem("redirect_uri", redirectUri());
    url.addQueryItem("response_type", "code");
    
    QStringList s = scopes();
    
    if (!s.isEmpty()) {
        url.addQueryItem("scope", s.join("+"));
    }
#endif
    return url;
}

QString Vimeo::userId() {
    return QSettings().value("Vimeo/userId").toString();
}

void Vimeo::setUserId(const QString &id) {
    Logger::log("Vimeo::setUserId(). ID: " + id, Logger::MediumVerbosity);
    
    if (id != userId()) {
        QSettings().setValue("Vimeo/userId", id);
        subscriptionCache.ids.clear();
        subscriptionCache.filters.clear();
        subscriptionCache.hasMore = true;

        if (self) {
            emit self->userIdChanged(id);
        }
    }
}

QString Vimeo::accessToken() {
    if (userId().isEmpty()) {
        return CLIENT_TOKEN;
    }
    
    QSqlQuery query = getDatabase().exec(QString("SELECT accessToken FROM vimeoAccounts WHERE userId = '%1'")
                                                .arg(userId()));
    
    if (query.lastError().isValid()) {
        Logger::log("Vimeo::accessToken(): database error: " + query.lastError().text());
        return QString();
    }
    
    if ((query.record().count() > 0) && (query.next())) {
         return query.value(0).toString();
    }
    
    return QString();
}

void Vimeo::setAccessToken(const QString &token) {
    Logger::log("Vimeo::setAccessToken(). Token: " + token, Logger::MediumVerbosity);
    QSqlQuery query = getDatabase().exec(QString("UPDATE vimeoAccounts SET accessToken = '%1' WHERE userId = '%2'")
                                                .arg(token).arg(userId()));

    if (query.lastError().isValid()) {
        Logger::log("Vimeo::setAccessToken(): database error: " + query.lastError().text());
    }
    else if (self) {
        emit self->accessTokenChanged(token);
    }
}

QString Vimeo::clientId() {
    return QSettings().value("Vimeo/clientId", CLIENT_ID).toString();
}

void Vimeo::setClientId(const QString &id) {
    Logger::log("Vimeo::setClientId(). ID: " + id, Logger::MediumVerbosity);
    
    if (id != clientId()) {
        QSettings().setValue("Vimeo/clientId", id);

        if (self) {
            emit self->clientIdChanged(id);
        }
    }
}

QString Vimeo::clientSecret() {
    return QSettings().value("Vimeo/clientSecret", CLIENT_SECRET).toString();
}

void Vimeo::setClientSecret(const QString &secret) {
    Logger::log("Vimeo::setClientSecret(). Secret: " + secret, Logger::MediumVerbosity);
    
    if (secret != clientSecret()) {
        QSettings().setValue("Vimeo/clientSecret", secret);

        if (self) {
            emit self->clientSecretChanged(secret);
        }
    }
}

QString Vimeo::redirectUri() {
    return QSettings().value("Vimeo/redirectUri", REDIRECT_URI).toString();
}

void Vimeo::setRedirectUri(const QString &uri) {
    Logger::log("Vimeo::setRedirectUri(). URI: " + uri, Logger::MediumVerbosity);
    
    if (uri != redirectUri()) {
        QSettings().setValue("Vimeo/redirectUri", uri);

        if (self) {
            emit self->redirectUriChanged(uri);
        }
    }
}

QStringList Vimeo::scopes() {
    return QSettings().value("Vimeo/scopes", SCOPES).toStringList();
}

void Vimeo::setScopes(const QStringList &s) {
    Logger::log("Vimeo::setScopes(). Scopes: " + s.join(", "), Logger::MediumVerbosity);
    
    if (s != scopes()) {
        QSettings().setValue("Vimeo/scopes", s);

        if (self) {
            emit self->scopesChanged(s);
        }
    }
}

bool Vimeo::hasScope(const QString &scope) {
    if (userId().isEmpty()) {
        return false;
    }
    
    QSqlQuery query = getDatabase().exec(QString("SELECT scopes FROM vimeoAccounts WHERE userId = '%1'")
                                                .arg(userId()));
    
    if (query.lastError().isValid()) {
        Logger::log("Vimeo::hasScope(): database error: " + query.lastError().text());
        return false;
    }
    
    if ((query.record().count() > 0) && (query.next())) {
         return query.value(0).toString().contains(scope);
    }
    
    return false;
}

QString Vimeo::createScope() {
    return QVimeo::CREATE_SCOPE;
}

QString Vimeo::deleteScope() {
    return QVimeo::DELETE_SCOPE;
}

QString Vimeo::editScope() {
    return QVimeo::EDIT_SCOPE;
}

QString Vimeo::interactScope() {
    return QVimeo::INTERACT_SCOPE;
}

QString Vimeo::publicScope() {
    return QVimeo::PUBLIC_SCOPE;
}

QString Vimeo::privateScope() {
    return QVimeo::PRIVATE_SCOPE;
}

QString Vimeo::purchasedScope() {
    return QVimeo::PURCHASED_SCOPE;
}

QString Vimeo::uploadScope() {
    return QVimeo::UPLOAD_SCOPE;
}
