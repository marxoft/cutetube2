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

#include "dailymotioncomment.h"
#include "dailymotion.h"
#include "dailymotion.h"
#include "logger.h"
#include "resources.h"
#include <QDateTime>

DailymotionComment::DailymotionComment(QObject *parent) :
    CTComment(parent),
    m_request(0)
{
    setService(Resources::DAILYMOTION);
}

DailymotionComment::DailymotionComment(const QString &id, QObject *parent) :
    CTComment(parent),
    m_request(0)
{
    setService(Resources::DAILYMOTION);
    loadComment(id);
}

DailymotionComment::DailymotionComment(const QVariantMap &comment, QObject *parent) :
    CTComment(parent),
    m_request(0)
{
    setService(Resources::DAILYMOTION);
    loadComment(comment);
}

DailymotionComment::DailymotionComment(const DailymotionComment *comment, QObject *parent) :
    CTComment(comment, parent),
    m_request(0)
{
}

QString DailymotionComment::errorString() const {
    return m_request ? Dailymotion::getErrorString(m_request->result().toMap()) : QString();
}

QDailymotion::ResourcesRequest::Status DailymotionComment::status() const {
    return m_request ? m_request->status() : QDailymotion::ResourcesRequest::Null;
}

void DailymotionComment::loadComment(const QString &id) {
    if (status() == QDailymotion::ResourcesRequest::Loading) {
        return;
    }
    
    initRequest();
    m_request->get("/comment/" + id, QVariantMap(), Dailymotion::COMMENT_FIELDS);
    connect(m_request, SIGNAL(finished()), this, SLOT(onCommentRequestFinished()));
    emit statusChanged(status());
}

void DailymotionComment::loadComment(const QVariantMap &comment) {
    setBody(comment.value("message").toString());
    setDate(QDateTime::fromTime_t(comment.value("created_time").toLongLong()).toString("dd MMM yyyy"));
    setId(comment.value("id").toString());
    setThumbnailUrl(comment.value("owner.avatar_60_url").toString());
    setUserId(comment.value("owner.id").toString());
    setUsername(comment.value("owner.screenname").toString());
    setVideoId(comment.value("video.id").toString());
}

void DailymotionComment::addComment() {
    if (status() == QDailymotion::ResourcesRequest::Loading) {
        return;
    }
    
    Logger::log("DailymotionComment::addComment()", Logger::MediumVerbosity);
    initRequest();
    QVariantMap resource;
    resource["message"] = body();
    m_request->insert(resource, QString("/video/%1/comments").arg(videoId()));
    connect(m_request, SIGNAL(finished()), this, SLOT(onAddCommentRequestFinished()));
    emit statusChanged(status());
}

void DailymotionComment::addComment(const QVariantMap &comment) {
    loadComment(comment);
    addComment();
}

void DailymotionComment::initRequest() {
    if (!m_request) {
        m_request = new QDailymotion::ResourcesRequest(this);
        m_request->setClientId(Dailymotion::clientId());
        m_request->setClientSecret(Dailymotion::clientSecret());
        m_request->setAccessToken(Dailymotion::accessToken());
        m_request->setRefreshToken(Dailymotion::refreshToken());
    
        connect(m_request, SIGNAL(accessTokenChanged(QString)), Dailymotion::instance(), SLOT(setAccessToken(QString)));
        connect(m_request, SIGNAL(refreshTokenChanged(QString)), Dailymotion::instance(), SLOT(setRefreshToken(QString)));
    }
}

void DailymotionComment::onCommentRequestFinished() {
    if (m_request->status() == QDailymotion::ResourcesRequest::Ready) {
        loadComment(m_request->result().toMap());
    }
    
    disconnect(m_request, SIGNAL(finished()), this, SLOT(onCommentRequestFinished()));
    emit statusChanged(status());
}

void DailymotionComment::onAddCommentRequestFinished() {
    if (m_request->status() == QDailymotion::ResourcesRequest::Ready) {
        loadComment(m_request->result().toMap());
        Logger::log("DailymotionComment::onAddCommentRequestFinished(). Comment added. ID: " + id(),
                    Logger::MediumVerbosity);
        emit Dailymotion::instance()->commentAdded(this);
    }
    else {
        Logger::log("DailymotionComment::onAddCommentRequestFinished(). Error: " + errorString());
    }
    
    disconnect(m_request, SIGNAL(finished()), this, SLOT(onAddCommentRequestFinished()));
    emit statusChanged(status());
}
