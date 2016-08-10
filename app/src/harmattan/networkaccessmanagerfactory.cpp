/*
 * Copyright (C) 2016 Stuart Howarth <showarth@marxoft.co.uk>
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms and conditions of the GNU General Public License,
 * version 3, as published by the Free Software Foundation.
 *
 * This program is distributed in the hope it will be useful, but WITHOUT ANY
 * WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software Foundation,
 * Inc., 51 Franklin St - Fifth Floor, Boston, MA 02110-1301 USA.
 */

#include "networkaccessmanagerfactory.h"
#include "cookiejar.h"
#include "networkaccessmanager.h"

NetworkAccessManagerFactory::NetworkAccessManagerFactory() :
    m_cookieJar(new CookieJar)
{
}

NetworkAccessManagerFactory::~NetworkAccessManagerFactory() {
    if (m_cookieJar) {
        delete m_cookieJar;
        m_cookieJar = 0;
    }
}

QNetworkCookieJar* NetworkAccessManagerFactory::cookieJar() const {
    return m_cookieJar;
}

QNetworkAccessManager* NetworkAccessManagerFactory::create(QObject *parent) {
    NetworkAccessManager *manager = new NetworkAccessManager(parent);

    if (!m_cookieJar) {
        m_cookieJar = new CookieJar;
    }

    manager->setCookieJar(m_cookieJar);
    m_cookieJar->setParent(0);

    return manager;
}
