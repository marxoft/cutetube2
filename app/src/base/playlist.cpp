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

Playlist::Playlist(QObject *parent) :
    QObject(parent)
{
}

Playlist::Playlist(const Playlist *playlist, QObject *parent) :
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

QString Playlist::date() const {
    return m_date;
}

void Playlist::setDate(const QString &d) {
    if (d != date()) {
        m_date = d;
        emit dateChanged();
    }
}

QString Playlist::description() const {
    return m_description;
}

void Playlist::setDescription(const QString &d) {
    if (d != description()) {
        m_description = d;
        emit descriptionChanged();
    }
}

QString Playlist::id() const {
    return m_id;
}

void Playlist::setId(const QString &i) {
    if (i != id()) {
        m_id = i;
        emit idChanged();
    }
}

QUrl Playlist::largeThumbnailUrl() const {
    return m_largeThumbnailUrl;
}

void Playlist::setLargeThumbnailUrl(const QUrl &u) {
    if (u != largeThumbnailUrl()) {
        m_largeThumbnailUrl = u;
        emit largeThumbnailUrlChanged();
    }
}

QString Playlist::service() const {
    return m_service;
}

void Playlist::setService(const QString &s) {
    if (s != service()) {
        m_service = s;
        emit serviceChanged();
    }
}

QUrl Playlist::thumbnailUrl() const {
    return m_thumbnailUrl;
}

void Playlist::setThumbnailUrl(const QUrl &u) {
    if (u != thumbnailUrl()) {
        m_thumbnailUrl = u;
        emit thumbnailUrlChanged();
    }
}

QString Playlist::title() const {
    return m_title;
}

void Playlist::setTitle(const QString &t) {
    if (t != title()) {
        m_title = t;
        emit titleChanged();
    }
}

QString Playlist::userId() const {
    return m_userId;
}

void Playlist::setUserId(const QString &i) {
    if (i != userId()) {
        m_userId = i;
        emit userIdChanged();
    }
}

QString Playlist::username() const {
    return m_username;
}

void Playlist::setUsername(const QString &u) {
    if (u != username()) {
        m_username = u;
        emit usernameChanged();
    }
}

int Playlist::videoCount() const {
    return m_videoCount;
}

void Playlist::setVideoCount(int c) {
    if (c != videoCount()) {
        m_videoCount = c;
        emit videoCountChanged();
    }
}

void Playlist::loadPlaylist(const Playlist *playlist) {
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
