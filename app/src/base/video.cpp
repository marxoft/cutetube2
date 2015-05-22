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

#include "video.h"

Video::Video(QObject *parent) :
    QObject(parent)
{
}

Video::Video(const Video *video, QObject *parent) :
    QObject(parent),
    m_date(video->date()),
    m_description(video->description()),
    m_duration(video->duration()),
    m_id(video->id()),
    m_largeThumbnailUrl(video->largeThumbnailUrl()),
    m_service(video->service()),
    m_streamUrl(video->streamUrl()),
    m_thumbnailUrl(video->thumbnailUrl()),
    m_title(video->title()),
    m_url(video->url()),
    m_userId(video->userId()),
    m_username(video->username()),
    m_viewCount(video->viewCount())
{
}

QString Video::date() const {
    return m_date;
}

void Video::setDate(const QString &d) {
    if (d != date()) {
        m_date = d;
        emit dateChanged();
    }
}

QString Video::description() const {
    return m_description;
}

void Video::setDescription(const QString &d) {
    if (d != description()) {
        m_description = d;
        emit descriptionChanged();
    }
}

QString Video::duration() const {
    return m_duration;
}

void Video::setDuration(const QString &d) {
    if (d != duration()) {
        m_duration = d;
        emit durationChanged();
    }
}

QString Video::id() const {
    return m_id;
}

void Video::setId(const QString &i) {
    if (i != id()) {
        m_id = i;
        emit idChanged();
    }
}

QUrl Video::largeThumbnailUrl() const {
    return m_largeThumbnailUrl;
}

void Video::setLargeThumbnailUrl(const QUrl &u) {
    if (u != largeThumbnailUrl()) {
        m_largeThumbnailUrl = u;
        emit largeThumbnailUrlChanged();
    }
}

QString Video::service() const {
    return m_service;
}

void Video::setService(const QString &s) {
    if (s != service()) {
        m_service = s;
        emit serviceChanged();
    }
}

QUrl Video::streamUrl() const {
    return m_streamUrl;
}

void Video::setStreamUrl(const QUrl &u) {
    if (u != streamUrl()) {
        m_streamUrl = u;
        emit streamUrlChanged();
    }
}

QUrl Video::thumbnailUrl() const {
    return m_thumbnailUrl;
}

void Video::setThumbnailUrl(const QUrl &u) {
    if (u != thumbnailUrl()) {
        m_thumbnailUrl = u;
        emit thumbnailUrlChanged();
    }
}

QString Video::title() const {
    return m_title;
}

void Video::setTitle(const QString &t) {
    if (t != title()) {
        m_title = t;
        emit titleChanged();
    }
}

QUrl Video::url() const {
    return m_url;
}

void Video::setUrl(const QUrl &u) {
    if (u != url()) {
        m_url = u;
        emit urlChanged();
    }
}

QString Video::userId() const {
    return m_userId;
}

void Video::setUserId(const QString &i) {
    if (i != userId()) {
        m_userId = i;
        emit userIdChanged();
    }
}

QString Video::username() const {
    return m_username;
}

void Video::setUsername(const QString &u) {
    if (u != username()) {
        m_username = u;
        emit usernameChanged();
    }
}

qint64 Video::viewCount() const {
    return m_viewCount;
}

void Video::setViewCount(qint64 c) {
    if (c != viewCount()) {
        m_viewCount = c;
        emit viewCountChanged();
    }
}

void Video::viewed() {
    setViewCount(viewCount() + 1);
}

void Video::loadVideo(Video *video) {
    setDate(video->date());
    setDescription(video->description());
    setDuration(video->duration());
    setId(video->id());
    setLargeThumbnailUrl(video->largeThumbnailUrl());
    setService(video->service());
    setStreamUrl(video->streamUrl());
    setThumbnailUrl(video->thumbnailUrl());
    setTitle(video->title());
    setUrl(video->url());
    setUserId(video->userId());
    setUsername(video->username());
    setViewCount(video->viewCount());
}
