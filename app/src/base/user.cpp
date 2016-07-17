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

#include "user.h"

CTUser::CTUser(QObject *parent) :
    QObject(parent)
{
}

CTUser::CTUser(const CTUser *user, QObject *parent) :
    QObject(parent),
    m_description(user->description()),
    m_id(user->id()),
    m_largeThumbnailUrl(user->largeThumbnailUrl()),
    m_service(user->service()),
    m_thumbnailUrl(user->thumbnailUrl()),
    m_username(user->username())
{
}

QString CTUser::description() const {
    return m_description;
}

void CTUser::setDescription(const QString &d) {
    if (d != description()) {
        m_description = d;
        emit descriptionChanged();
    }
}

QString CTUser::id() const {
    return m_id;
}

void CTUser::setId(const QString &i) {
    if (i != id()) {
        m_id = i;
        emit idChanged();
    }
}

QUrl CTUser::largeThumbnailUrl() const {
    return m_largeThumbnailUrl;
}

void CTUser::setLargeThumbnailUrl(const QUrl &u) {
    if (u != largeThumbnailUrl()) {
        m_largeThumbnailUrl = u;
        emit largeThumbnailUrlChanged();
    }
}

QString CTUser::service() const {
    return m_service;
}

void CTUser::setService(const QString &s) {
    if (s != service()) {
        m_service = s;
        emit serviceChanged();
    }
}

QUrl CTUser::thumbnailUrl() const {
    return m_thumbnailUrl;
}

void CTUser::setThumbnailUrl(const QUrl &u) {
    if (u != thumbnailUrl()) {
        m_thumbnailUrl = u;
        emit thumbnailUrlChanged();
    }
}

QString CTUser::username() const {
    return m_username;
}

void CTUser::setUsername(const QString &u) {
    if (u != username()) {
        m_username = u;
        emit usernameChanged();
    }
}

void CTUser::loadUser(CTUser *user) {
    setDescription(user->description());
    setId(user->id());
    setLargeThumbnailUrl(user->largeThumbnailUrl());
    setService(user->service());
    setThumbnailUrl(user->thumbnailUrl());
    setUsername(user->username());
}
