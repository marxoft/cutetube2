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

#include "comment.h"

CTComment::CTComment(QObject *parent) :
    QObject(parent)
{
}

CTComment::CTComment(const CTComment *comment, QObject *parent) :
    QObject(parent),
    m_body(comment->body()),
    m_date(comment->date()),
    m_id(comment->id()),
    m_service(comment->service()),
    m_thumbnailUrl(comment->thumbnailUrl()),
    m_userId(comment->userId()),
    m_username(comment->username()),
    m_videoId(comment->videoId())
{
}

QString CTComment::body() const {
    return m_body;
}

void CTComment::setBody(const QString &d) {
    if (d != body()) {
        m_body = d;
        emit bodyChanged();
    }
}

QString CTComment::date() const {
    return m_date;
}

void CTComment::setDate(const QString &d) {
    if (d != date()) {
        m_date = d;
        emit dateChanged();
    }
}

QString CTComment::id() const {
    return m_id;
}

void CTComment::setId(const QString &i) {
    if (i != id()) {
        m_id = i;
        emit idChanged();
    }
}

QString CTComment::service() const {
    return m_service;
}

void CTComment::setService(const QString &s) {
    if (s != service()) {
        m_service = s;
        emit serviceChanged();
    }
}

QUrl CTComment::thumbnailUrl() const {
    return m_thumbnailUrl;
}

void CTComment::setThumbnailUrl(const QUrl &u) {
    if (u != thumbnailUrl()) {
        m_thumbnailUrl = u;
        emit thumbnailUrlChanged();
    }
}

QString CTComment::userId() const {
    return m_userId;
}

void CTComment::setUserId(const QString &i) {
    if (i != userId()) {
        m_userId = i;
        emit userIdChanged();
    }
}

QString CTComment::username() const {
    return m_username;
}

void CTComment::setUsername(const QString &u) {
    if (u != username()) {
        m_username = u;
        emit usernameChanged();
    }
}

QString CTComment::videoId() const {
    return m_videoId;
}

void CTComment::setVideoId(const QString &i) {
    if (i != videoId()) {
        m_videoId = i;
        emit videoIdChanged();
    }
}

void CTComment::loadComment(CTComment *comment) {
    setBody(comment->body());
    setDate(comment->date());
    setId(comment->id());
    setService(comment->service());
    setThumbnailUrl(comment->thumbnailUrl());
    setUserId(comment->userId());
    setUsername(comment->username());
    setVideoId(comment->videoId());
}
