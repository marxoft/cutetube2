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

#include "plugincomment.h"
#include "resources.h"

PluginComment::PluginComment(QObject *parent) :
    CTComment(parent),
    m_request(new ResourcesRequest(this))
{
    connect(m_request, SIGNAL(finished()), this, SLOT(onRequestFinished()));
}

PluginComment::PluginComment(const QString &service, const QString &id, QObject *parent) :
    CTComment(parent),
    m_request(new ResourcesRequest(this))
{
    loadComment(service, id);
    connect(m_request, SIGNAL(finished()), this, SLOT(onRequestFinished()));
}

PluginComment::PluginComment(const QString &service, const QVariantMap &comment, QObject *parent) :
    CTComment(parent),
    m_request(new ResourcesRequest(this))
{
    loadComment(service, comment);
    connect(m_request, SIGNAL(finished()), this, SLOT(onRequestFinished()));
}

PluginComment::PluginComment(const PluginComment *comment, QObject *parent) :
    CTComment(comment, parent),
    m_request(new ResourcesRequest(this))
{
}

QString PluginComment::errorString() const {
    return m_request->errorString();
}

ResourcesRequest::Status PluginComment::status() const {
    return m_request->status();
}

void PluginComment::loadComment(const QString &service, const QString &id) {
    if (status() == ResourcesRequest::Loading) {
        return;
    }
    
    m_request->setService(service);
    m_request->get(Resources::COMMENT, id);

    emit statusChanged(status());
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

void PluginComment::onRequestFinished() {
    if (m_request->status() == ResourcesRequest::Ready) {
        loadComment(m_request->service(), m_request->result().toMap());
    }
    
    emit statusChanged(status());
}
