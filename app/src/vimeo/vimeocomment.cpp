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

#include "vimeocomment.h"
#include "logger.h"
#include "resources.h"
#include "vimeo.h"
#include <QDateTime>

VimeoComment::VimeoComment(QObject *parent) :
    CTComment(parent),
    m_request(0)
{
    setService(Resources::VIMEO);
}

VimeoComment::VimeoComment(const QString &id, QObject *parent) :
    CTComment(parent),
    m_request(0)
{
    setService(Resources::VIMEO);
    loadComment(id);
}

VimeoComment::VimeoComment(const QVariantMap &comment, QObject *parent) :
    CTComment(parent),
    m_request(0)
{
    setService(Resources::VIMEO);
    loadComment(comment);
}

VimeoComment::VimeoComment(const VimeoComment *comment, QObject *parent) :
    CTComment(comment, parent),
    m_request(0)
{
}

QString VimeoComment::errorString() const {
    return m_request ? Vimeo::getErrorString(m_request->result().toMap()) : QString();
}

QVimeo::ResourcesRequest::Status VimeoComment::status() const {
    return m_request ? m_request->status() : QVimeo::ResourcesRequest::Null;
}

void VimeoComment::loadComment(const QString &id) {
    if (status() == QVimeo::ResourcesRequest::Loading) {
        return;
    }
    
    initRequest();
    m_request->get(id);
    connect(m_request, SIGNAL(finished()), this, SLOT(onCommentRequestFinished()));
    emit statusChanged(status());
}

void VimeoComment::loadComment(const QVariantMap &comment) {
    const QVariantMap user = comment.value("user").toMap();
    
    setBody(comment.value("text").toString());
    setDate(QDateTime::fromString(comment.value("created_on").toString(), Qt::ISODate).toString("dd MMM yyyy"));
    setId(comment.value("uri").toString().section('/', -1));
    setUserId(user.value("uri").toString().section('/', -1));
    setThumbnailUrl(QString("https://i.vimeocdn.com/portrait/%1_75x75.jpg").arg(userId()));
    setUsername(user.value("name").toString());
    setVideoId(comment.value("uri").toString().section('/', 1));
}

void VimeoComment::addComment() {
    if (status() == QVimeo::ResourcesRequest::Loading) {
        return;
    }
    
    Logger::log("VimeoComment::addComment()", Logger::MediumVerbosity);
    initRequest();
    QVariantMap resource;
    resource["text"] = body();
    m_request->insert(resource, QString("/videos/%1/comments").arg(videoId()));
    connect(m_request, SIGNAL(finished()), this, SLOT(onAddCommentRequestFinished()));
    emit statusChanged(status());
}

void VimeoComment::addComment(const QVariantMap &comment) {
    loadComment(comment);
    addComment();
}

void VimeoComment::initRequest() {
    if (!m_request) {
        m_request = new QVimeo::ResourcesRequest(this);
        m_request->setClientId(Vimeo::clientId());
        m_request->setClientSecret(Vimeo::clientSecret());
        m_request->setAccessToken(Vimeo::accessToken());
    
        connect(m_request, SIGNAL(accessTokenChanged(QString)), Vimeo::instance(), SLOT(setAccessToken(QString)));
    }
}

void VimeoComment::onCommentRequestFinished() {
    if (m_request->status() == QVimeo::ResourcesRequest::Ready) {
        loadComment(m_request->result().toMap());
    }
    
    disconnect(m_request, SIGNAL(finished()), this, SLOT(onCommentRequestFinished()));
    emit statusChanged(status());
}

void VimeoComment::onAddCommentRequestFinished() {
    if (m_request->status() == QVimeo::ResourcesRequest::Ready) {
        loadComment(m_request->result().toMap());
        Logger::log("VimeoComment::onAddCommentRequestFinished(). Comment added. ID: " + id(), Logger::MediumVerbosity);
        emit Vimeo::instance()->commentAdded(this);
    }
    else {
        Logger::log("VimeoComment::onAddCommentRequestFinished(). Error: " + errorString());
    }
    
    disconnect(m_request, SIGNAL(finished()), this, SLOT(onAddCommentRequestFinished()));
    emit statusChanged(status());
}
