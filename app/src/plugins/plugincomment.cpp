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

#include "plugincomment.h"
#include "pluginmanager.h"
#include "resources.h"

PluginComment::PluginComment(QObject *parent) :
    CTComment(parent),
    m_request(0)
{
}

PluginComment::PluginComment(const QString &service, const QString &id, QObject *parent) :
    CTComment(parent),
    m_request(0)
{
    loadComment(service, id);
}

PluginComment::PluginComment(const QString &service, const QVariantMap &comment, QObject *parent) :
    CTComment(parent),
    m_request(0)
{
    loadComment(service, comment);
}

PluginComment::PluginComment(const PluginComment *comment, QObject *parent) :
    CTComment(comment, parent),
    m_request(0)
{
}

QString PluginComment::errorString() const {
    return m_request ? m_request->errorString() : QString();
}

ResourcesRequest::Status PluginComment::status() const {
    return m_request ? m_request->status() : ResourcesRequest::Null;
}

void PluginComment::loadComment(const QString &service, const QString &id) {
    setService(service);
    
    if (status() == ResourcesRequest::Loading) {
        return;
    }

    if (ResourcesRequest *r = request()) {
        r->get(Resources::COMMENT, id);    
        emit statusChanged(status());
    }
}

void PluginComment::loadComment(const QString &service, const QVariantMap &comment) {
    setService(service);
    setBody(comment.value("body").toString());
    setDate(comment.value("date").toString());
    setId(comment.value("id").toString());
    setThumbnailUrl(comment.value("thumbnailUrl").toString());
    setUserId(comment.value("userId").toString());
    setUsername(comment.value("username").toString());
    setVideoId(comment.value("videoId").toString());
}

void PluginComment::loadComment(PluginComment *comment) {
    CTComment::loadComment(comment);
}

ResourcesRequest* PluginComment::request() {
    if (!m_request) {
        m_request = PluginManager::instance()->createRequestForService(service(), this);

        if (m_request) {
            connect(m_request, SIGNAL(finished()), this, SLOT(onRequestFinished()));
        }
    }

    return m_request;
}

void PluginComment::onRequestFinished() {
    if (m_request->status() == ResourcesRequest::Ready) {
        loadComment(service(), m_request->result().toMap());
    }
    
    emit statusChanged(status());
}
