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

#include "vimeo.h"
#include "database.h"
#include <qvimeo/urls.h>
#include <QSettings>
#include <QSqlRecord>
#if QT_VERSION >= 0x050000
#include <QUrlQuery>
#endif
#ifdef CUTETUBE_DEBUG
#include <QDebug>
#endif

static const QString CLIENT_ID("0bf284bf5a0e46630f5097a590a76ef976a94322");
static const QString CLIENT_SECRET("7nnZ1OPS13hjKAVhzuXx/4AIdKxmgDNasHkj5QraWWkrNsd6mxYWZG73AKaFUdLzoNWhGA75jSffs+JyAFfi\
0MiFi1OXnzHsxaL0HCIFpxk0GpZlXcScWmJTHvGGtVv1");
static const QString CLIENT_TOKEN("fb5c66ebe6938e858921963f850355a0");
static const QString REDIRECT_URI("http://marxoft.co.uk/projects/cutetube");
static const QStringList SCOPES = QStringList() << QVimeo::PUBLIC_SCOPE << QVimeo::PRIVATE_SCOPE << QVimeo::CREATE_SCOPE
                                                << QVimeo::EDIT_SCOPE << QVimeo::DELETE_SCOPE << QVimeo::INTERACT_SCOPE;

const QRegExp Vimeo::URL_REGEXP("http(s|)://vimeo.com/\\w+", Qt::CaseInsensitive);

Vimeo::SubscriptionCache Vimeo::subscriptionCache;

Vimeo* Vimeo::self = 0;

Vimeo::Vimeo(QObject *parent) :
    QObject(parent)
{
    if (!self) {
        self = this;
    }
}

Vimeo::~Vimeo() {
    if (self == this) {
        self = 0;
    }
}

Vimeo* Vimeo::instance() {
    return self;
}

QString Vimeo::getErrorString(const QVariantMap &error) {
    return error.contains("error") ? error.value("error").toString() : tr("Unknown error");
}

QUrl Vimeo::authUrl() const {
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

QString Vimeo::userId() const {
    return QSettings().value("Vimeo/userId").toString();
}

void Vimeo::setUserId(const QString &id) {
    if (id != userId()) {
        QSettings().setValue("Vimeo/userId", id);
        subscriptionCache.ids.clear();
        subscriptionCache.filters.clear();
        subscriptionCache.hasMore = false;
        emit userIdChanged();
    }
}

QString Vimeo::accessToken() const {
    if (userId().isEmpty()) {
        return CLIENT_TOKEN;
    }
    
    QSqlQuery query = getDatabase().exec(QString("SELECT accessToken FROM vimeoAccounts WHERE userId = '%1'")
                                                .arg(userId()));
    
    if (query.lastError().isValid()) {
        qDebug() << "Vimeo::accessToken: database error:" << query.lastError().text();
        return QString();
    }
    
    if ((query.record().count() > 0) && (query.next())) {
         return query.value(0).toString();
    }
    
    return QString();
}

void Vimeo::setAccessToken(const QString &token) {
    QSqlQuery query = getDatabase().exec(QString("UPDATE vimeoAccounts SET accessToken = '%1' WHERE userId = '%2'")
                                                .arg(token).arg(userId()));

    if (query.lastError().isValid()) {
        qDebug() << "Vimeo::setAccessToken: database error:" << query.lastError().text();
    }
    else {
        emit accessTokenChanged();
    }
#ifdef CUTETUBE_DEBUG
    qDebug() << "Vimeo::setAccessToken" << token;
#endif
}

QString Vimeo::clientId() const {
    return QSettings().value("Vimeo/clientId", CLIENT_ID).toString();
}

void Vimeo::setClientId(const QString &id) {
    if (id != clientId()) {
        QSettings().setValue("Vimeo/clientId", id);
        emit clientIdChanged();
    }
#ifdef CUTETUBE_DEBUG
    qDebug() << "Vimeo::setClientId" << id;
#endif
}

QString Vimeo::clientSecret() const {
    return QSettings().value("Vimeo/clientSecret", CLIENT_SECRET).toString();
}

void Vimeo::setClientSecret(const QString &secret) {
    if (secret != clientSecret()) {
        QSettings().setValue("Vimeo/clientSecret", secret);
        emit clientSecretChanged();
    }
#ifdef CUTETUBE_DEBUG
    qDebug() << "Vimeo::setClientSecret" << secret;
#endif
}

QString Vimeo::redirectUri() const {
    return QSettings().value("Vimeo/redirectUri", REDIRECT_URI).toString();
}

void Vimeo::setRedirectUri(const QString &uri) {
    if (uri != redirectUri()) {
        QSettings().setValue("Vimeo/redirectUri", uri);
        emit redirectUriChanged();
    }
#ifdef CUTETUBE_DEBUG
    qDebug() << "Vimeo::setRedirectUri" << uri;
#endif
}

QStringList Vimeo::scopes() const {
    return QSettings().value("Vimeo/scopes", SCOPES).toStringList();
}

void Vimeo::setScopes(const QStringList &s) {
    if (s != scopes()) {
        QSettings().setValue("Vimeo/scopes", s);
        emit scopesChanged();
    }
#ifdef CUTETUBE_DEBUG
    qDebug() << "Vimeo::setScopes" << s;
#endif
}

bool Vimeo::hasScope(const QString &scope) const {
    if (userId().isEmpty()) {
        return false;
    }
    
    QSqlQuery query = getDatabase().exec(QString("SELECT scopes FROM vimeoAccounts WHERE userId = '%1'")
                                                .arg(userId()));
    
    if (query.lastError().isValid()) {
        qDebug() << "Vimeo::hasScope: database error:" << query.lastError().text();
        return false;
    }
    
    if ((query.record().count() > 0) && (query.next())) {
         return query.value(0).toString().contains(scope);
    }
    
    return false;
}
