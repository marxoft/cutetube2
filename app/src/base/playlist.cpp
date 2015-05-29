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

#include "playlist.h"

CTPlaylist::CTPlaylist(QObject *parent) :
    QObject(parent)
{
}

CTPlaylist::CTPlaylist(const CTPlaylist *playlist, QObject *parent) :
    QObject(parent),
    m_date(playlist->date()),
    m_description(playlist->description()),
    m_id(playlist->id()),
    m_largeThumbnailUrl(playlist->largeThumbnailUrl()),
    m_service(playlist->service()),
    m_thumbnailUrl(playlist->thumbnailUrl()),
    m_title(playlist->title()),
    m_userId(playlist->userId()),
    m_username(playlist->username()),
    m_videoCount(playlist->videoCount())
{
}

QString CTPlaylist::date() const {
    return m_date;
}

void CTPlaylist::setDate(const QString &d) {
    if (d != date()) {
        m_date = d;
        emit dateChanged();
    }
}

QString CTPlaylist::description() const {
    return m_description;
}

void CTPlaylist::setDescription(const QString &d) {
    if (d != description()) {
        m_description = d;
        emit descriptionChanged();
    }
}

QString CTPlaylist::id() const {
    return m_id;
}

void CTPlaylist::setId(const QString &i) {
    if (i != id()) {
        m_id = i;
        emit idChanged();
    }
}

QUrl CTPlaylist::largeThumbnailUrl() const {
    return m_largeThumbnailUrl;
}

void CTPlaylist::setLargeThumbnailUrl(const QUrl &u) {
    if (u != largeThumbnailUrl()) {
        m_largeThumbnailUrl = u;
        emit largeThumbnailUrlChanged();
    }
}

QString CTPlaylist::service() const {
    return m_service;
}

void CTPlaylist::setService(const QString &s) {
    if (s != service()) {
        m_service = s;
        emit serviceChanged();
    }
}

QUrl CTPlaylist::thumbnailUrl() const {
    return m_thumbnailUrl;
}

void CTPlaylist::setThumbnailUrl(const QUrl &u) {
    if (u != thumbnailUrl()) {
        m_thumbnailUrl = u;
        emit thumbnailUrlChanged();
    }
}

QString CTPlaylist::title() const {
    return m_title;
}

void CTPlaylist::setTitle(const QString &t) {
    if (t != title()) {
        m_title = t;
        emit titleChanged();
    }
}

QString CTPlaylist::userId() const {
    return m_userId;
}

void CTPlaylist::setUserId(const QString &i) {
    if (i != userId()) {
        m_userId = i;
        emit userIdChanged();
    }
}

QString CTPlaylist::username() const {
    return m_username;
}

void CTPlaylist::setUsername(const QString &u) {
    if (u != username()) {
        m_username = u;
        emit usernameChanged();
    }
}

int CTPlaylist::videoCount() const {
    return m_videoCount;
}

void CTPlaylist::setVideoCount(int c) {
    if (c != videoCount()) {
        m_videoCount = c;
        emit videoCountChanged();
    }
}

void CTPlaylist::loadPlaylist(CTPlaylist *playlist) {
    setDate(playlist->date());
    setDescription(playlist->description());
    setId(playlist->id());
    setLargeThumbnailUrl(playlist->largeThumbnailUrl());
    setService(playlist->service());
    setThumbnailUrl(playlist->thumbnailUrl());
    setTitle(playlist->title());
    setUserId(playlist->userId());
    setUsername(playlist->username());
    setVideoCount(playlist->videoCount());
}
