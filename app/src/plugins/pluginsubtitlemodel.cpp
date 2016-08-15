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

#include "pluginsubtitlemodel.h"
#include "dailymotion.h"
#include "logger.h"
#include "pluginmanager.h"
#include "resources.h"
#include "vimeo.h"
#include <qdailymotion/resourcesrequest.h>
#include <qvimeo/resourcesrequest.h>
#include <qyoutube/subtitlesrequest.h>

PluginSubtitleModel::PluginSubtitleModel(QObject *parent) :
    SelectionModel(parent),
    m_request(0),
    m_dailymotionRequest(0),
    m_vimeoRequest(0),
    m_youtubeRequest(0),
    m_status(ResourcesRequest::Null)
{
}

QString PluginSubtitleModel::errorString() const {
    return m_request ? m_request->errorString() : QString();
}

void PluginSubtitleModel::setErrorString(const QString &e) {
    m_errorString = e;
}

QString PluginSubtitleModel::service() const {
    return m_service;
}

void PluginSubtitleModel::setService(const QString &s) {
    if (s != service()) {
        m_service = s;
        emit serviceChanged();

        clear();

        if (m_request) {
            m_request->deleteLater();
            m_request = 0;
        }
    }
}

ResourcesRequest::Status PluginSubtitleModel::status() const {
    return m_status;
}

void PluginSubtitleModel::setStatus(ResourcesRequest::Status s) {
    if (s != status()) {
        m_status = s;
        emit statusChanged(s);
    }
}

void PluginSubtitleModel::list(const QString &resourceId) {
    if (status() == ResourcesRequest::Loading) {
        return;
    }
    
    Logger::log("PluginSubtitleModel::list(). Resource ID: " + resourceId, Logger::MediumVerbosity);
    clear();
    m_resourceId = resourceId;

    if (ResourcesRequest *r = request()) {
        r->list(Resources::SUBTITLE, resourceId);
        setStatus(r->status());
    }
}

void PluginSubtitleModel::cancel() {
    if (m_request) {
        m_request->cancel();
    }
}

void PluginSubtitleModel::reload() {
    if (status() == ResourcesRequest::Loading) {
        return;
    }
    
    Logger::log("PluginSubtitleModel::reload(). Resource ID: " + m_resourceId, Logger::MediumVerbosity);
    clear();

    if (ResourcesRequest *r = request()) {
        r->list(Resources::SUBTITLE, m_resourceId);
        setStatus(r->status());
    }
}

ResourcesRequest* PluginSubtitleModel::request() {
    if (!m_request) {
        m_request = PluginManager::instance()->createRequestForService(service(), this);

        if (m_request) {
            connect(m_request, SIGNAL(finished()), this, SLOT(onRequestFinished()));
        }
    }

    return m_request;
}

QDailymotion::ResourcesRequest* PluginSubtitleModel::dailymotionRequest() {
    if (!m_dailymotionRequest) {
        m_dailymotionRequest = new QDailymotion::ResourcesRequest(this);
        m_dailymotionRequest->setClientId(Dailymotion::clientId());
        m_dailymotionRequest->setClientSecret(Dailymotion::clientSecret());
        m_dailymotionRequest->setAccessToken(Dailymotion::accessToken());
        m_dailymotionRequest->setRefreshToken(Dailymotion::refreshToken());
        
        connect(m_dailymotionRequest, SIGNAL(accessTokenChanged(QString)),
                Dailymotion::instance(), SLOT(setAccessToken(QString)));
        connect(m_dailymotionRequest, SIGNAL(refreshTokenChanged(QString)),
                Dailymotion::instance(), SLOT(setRefreshToken(QString)));
        connect(m_dailymotionRequest, SIGNAL(finished()), this, SLOT(onDailymotionRequestFinished()));
    }
    
    return m_dailymotionRequest;
}

QVimeo::ResourcesRequest* PluginSubtitleModel::vimeoRequest() {
    if (!m_vimeoRequest) {
        m_vimeoRequest = new QVimeo::ResourcesRequest(this);
        m_vimeoRequest->setClientId(Vimeo::clientId());
        m_vimeoRequest->setClientSecret(Vimeo::clientSecret());
        m_vimeoRequest->setAccessToken(Vimeo::accessToken());
        
        connect(m_vimeoRequest, SIGNAL(accessTokenChanged(QString)), Vimeo::instance(), SLOT(setAccessToken(QString)));
        connect(m_vimeoRequest, SIGNAL(finished()), this, SLOT(onVimeoRequestFinished()));
    }
    
    return m_vimeoRequest;
}

QYouTube::SubtitlesRequest* PluginSubtitleModel::youtubeRequest() {
    if (!m_youtubeRequest) {
        m_youtubeRequest = new QYouTube::SubtitlesRequest(this);
        connect(m_youtubeRequest, SIGNAL(finished()), this, SLOT(onYouTubeRequestFinished()));
    }
    
    return m_youtubeRequest;
}

void PluginSubtitleModel::onRequestFinished() {
    if (m_request->status() == ResourcesRequest::Ready) {
        const QVariantMap result = m_request->result().toMap();
        
        if (result.contains("items")) {
            foreach (const QVariant &v, result.value("items").toList()) {
                const QVariantMap subtitle = v.toMap();
                append(subtitle.value("title").toString(), subtitle);
            }
            
            setErrorString(QString());
            setStatus(ResourcesRequest::Ready);
        }
        else if (result.contains("service")) {
            const QString service = result.value("service").toString();
            
            if (service == Resources::DAILYMOTION) {
                dailymotionRequest()->list(QString("/video/%1/subtitles").arg(result.value("id").toString()),
                                           QVariantMap(), Dailymotion::SUBTITLE_FIELDS);
            }
            else if (service == Resources::VIMEO) {
                vimeoRequest()->list(QString("/videos/%1/texttracks").arg(result.value("id").toString()));
            }
            else if (service == Resources::YOUTUBE) {
                youtubeRequest()->list(result.value("id").toString());
            }
            else {
                setErrorString(tr("Attempted redirect to unsupported service '%1'").arg(service));
                setStatus(ResourcesRequest::Failed);
                Logger::log("PluginSubtitleModel::onRequestFinished(). Error: " + errorString());
            }
        }
        else {
            setErrorString(tr("Unknown error"));
            setStatus(ResourcesRequest::Failed);
            Logger::log("PluginSubtitleModel::onRequestFinished(). Error: " + errorString());
        }
    }
    else {
        setErrorString(m_request->errorString());
        setStatus(ResourcesRequest::Failed);
        Logger::log("PluginSubtitleModel::onRequestFinished(). Error: " + errorString());
    }
}

void PluginSubtitleModel::onDailymotionRequestFinished() {
    if (m_dailymotionRequest->status() == QDailymotion::ResourcesRequest::Ready) {
        foreach (const QVariant &v, m_dailymotionRequest->result().toMap().value("list").toList()) {
            const QVariantMap subtitle = v.toMap();
            append(subtitle.value("language").toString(), subtitle);
        }
        
        setErrorString(QString());
        setStatus(ResourcesRequest::Ready);
    }
    else {
        setErrorString(m_dailymotionRequest->errorString());
        setStatus(ResourcesRequest::Failed);
        Logger::log("PluginSubtitleModel::onDailymotionRequestFinished(). Error: " + errorString());
    }
}

void PluginSubtitleModel::onVimeoRequestFinished() {
    if (m_vimeoRequest->status() == QVimeo::ResourcesRequest::Ready) {
        foreach (const QVariant &v, m_vimeoRequest->result().toMap().value("items").toList()) {
            const QVariantMap subtitle = v.toMap();
            append(subtitle.value("name").toString(), subtitle);
        }
        
        setErrorString(QString());
        setStatus(ResourcesRequest::Ready);
    }
    else {
        setErrorString(m_vimeoRequest->errorString());
        setStatus(ResourcesRequest::Failed);
        Logger::log("PluginSubtitleModel::onVimeoRequestFinished(). Error: " + errorString());
    }
}

void PluginSubtitleModel::onYouTubeRequestFinished() {
    if (m_youtubeRequest->status() == QYouTube::SubtitlesRequest::Ready) {
        foreach (const QVariant &v, m_youtubeRequest->result().toList()) {
            const QVariantMap subtitle = v.toMap();
            append(subtitle.value("translatedLanguage").toString(), subtitle);
        }
        
        setErrorString(QString());
        setStatus(ResourcesRequest::Ready);
    }
    else {
        setErrorString(m_youtubeRequest->errorString());
        setStatus(ResourcesRequest::Failed);
        Logger::log("PluginSubtitleModel::onYouTubeRequestFinished(). Error: " + errorString());
    }
}
