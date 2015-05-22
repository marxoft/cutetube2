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

#include "user.h"

User::User(QObject *parent) :
    QObject(parent)
{
}

User::User(const User *user, QObject *parent) :
    QObject(parent),
    m_description(user->description()),
    m_id(user->id()),
    m_largeThumbnailUrl(user->largeThumbnailUrl()),
    m_service(user->service()),
    m_thumbnailUrl(user->thumbnailUrl()),
    m_username(user->username())
{
}

QString User::description() const {
    return m_description;
}

void User::setDescription(const QString &d) {
    if (d != description()) {
        m_description = d;
        emit descriptionChanged();
    }
}

QString User::id() const {
    return m_id;
}

void User::setId(const QString &i) {
    if (i != id()) {
        m_id = i;
        emit idChanged();
    }
}

QUrl User::largeThumbnailUrl() const {
    return m_largeThumbnailUrl;
}

void User::setLargeThumbnailUrl(const QUrl &u) {
    if (u != largeThumbnailUrl()) {
        m_largeThumbnailUrl = u;
        emit largeThumbnailUrlChanged();
    }
}

QString User::service() const {
    return m_service;
}

void User::setService(const QString &s) {
    if (s != service()) {
        m_service = s;
        emit serviceChanged();
    }
}

QUrl User::thumbnailUrl() const {
    return m_thumbnailUrl;
}

void User::setThumbnailUrl(const QUrl &u) {
    if (u != thumbnailUrl()) {
        m_thumbnailUrl = u;
        emit thumbnailUrlChanged();
    }
}

QString User::username() const {
    return m_username;
}

void User::setUsername(const QString &u) {
    if (u != username()) {
        m_username = u;
        emit usernameChanged();
    }
}

void User::loadUser(User *user) {
    setDescription(user->description());
    setId(user->id());
    setLargeThumbnailUrl(user->largeThumbnailUrl());
    setService(user->service());
    setThumbnailUrl(user->thumbnailUrl());
    setUsername(user->username());
}
