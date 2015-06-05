/*
 * Copyright (C) 2015 Stuart Howarth <showarth@marxoft.co.uk>
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms and conditions of the GNU Lesser General Public License,
 * version 3, as published by the Free Software Foundation.
 *
 * This program is distributed in the hope it will be useful, but WITHOUT ANY
 * WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for
 * more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program; if not, write to the Free Software Foundation,
 * Inc., 51 Franklin St - Fifth Floor, Boston, MA 02110-1301 USA.
 */

#include "cookiejar.h"
#include <QNetworkCookie>
#ifdef CUTETUBE_DEBUG
#include <QDebug>
#endif

CookieJar::CookieJar(QObject *parent) :
    QNetworkCookieJar(parent)
{
}

void CookieJar::setAllCookies(const QVariantList &cookies) {
#ifdef CUTETUBE_DEBUG
    qDebug() << "CookieJar::setAllCookies" << cookies;
#endif
    QList<QNetworkCookie> list;

    foreach (QVariant v, cookies) {
        QVariantMap cookie = v.toMap();
        list << QNetworkCookie(cookie.value("name").toString().toUtf8(), cookie.value("value").toString().toUtf8());
    }

    QNetworkCookieJar::setAllCookies(list);
}

bool CookieJar::setCookiesFromUrl(const QVariantList &cookies, const QUrl &url) {
#ifdef CUTETUBE_DEBUG
    qDebug() << "CookieJar::setCookiesFromUrl" << cookies << url;
#endif
    QList<QNetworkCookie> list;

    foreach (QVariant v, cookies) {
        QVariantMap cookie = v.toMap();
        list << QNetworkCookie(cookie.value("name").toString().toUtf8(), cookie.value("value").toString().toUtf8());
    }

    return QNetworkCookieJar::setCookiesFromUrl(list, url);
}
