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

#include "pluginstreammodel.h"
#include "logger.h"
#include "pluginmanager.h"
#include "resources.h"
#include <qdailymotion/streamsrequest.h>
#include <qvimeo/streamsrequest.h>
#include <qyoutube/streamsrequest.h>

PluginStreamModel::PluginStreamModel(QObject *parent) :
    SelectionModel(parent),
    m_request(0),
    m_dailymotionRequest(0),
    m_vimeoRequest(0),
    m_youtubeRequest(0),
    m_status(ResourcesRequest::Null)
{
}

QString PluginStreamModel::errorString() const {
    return m_request ? m_request->errorString() : QString();
}

void PluginStreamModel::setErrorString(const QString &e) {
    m_errorString = e;
}

QString PluginStreamModel::service() const {
    return m_service;
}

void PluginStreamModel::setService(const QString &s) {
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

ResourcesRequest::Status PluginStreamModel::status() const {
    return m_status;
}

void PluginStreamModel::setStatus(ResourcesRequest::Status s) {
    if (s != status()) {
        m_status = s;
        emit statusChanged(s);
    }
}

void PluginStreamModel::list(const QString &resourceId) {
    if (status() == ResourcesRequest::Loading) {
        return;
    }
    
    Logger::log("PluginStreamModel::list(). Resource ID: " + resourceId, Logger::MediumVerbosity);
    clear();
    m_resourceId = resourceId;

    if (ResourcesRequest *r = request()) {
        r->list(Resources::STREAM, resourceId);
        setStatus(r->status());
    }
}

void PluginStreamModel::cancel() {
    if (m_request) {
        m_request->cancel();
    }
}

void PluginStreamModel::reload() {
    if (status() == ResourcesRequest::Loading) {
        return;
    }
    
    Logger::log("PluginStreamModel::reload(). Resource ID: " + m_resourceId, Logger::MediumVerbosity);
    clear();

    if (ResourcesRequest *r = request()) {
        r->list(Resources::STREAM, m_resourceId);
        setStatus(r->status());
    }
}

ResourcesRequest* PluginStreamModel::request() {
    if (!m_request) {
        m_request = PluginManager::instance()->createRequestForService(service(), this);

        if (m_request) {
            connect(m_request, SIGNAL(finished()), this, SLOT(onRequestFinished()));
        }
    }

    return m_request;
}

QDailymotion::StreamsRequest* PluginStreamModel::dailymotionRequest() {
    if (!m_dailymotionRequest) {
        m_dailymotionRequest = new QDailymotion::StreamsRequest(this);
        connect(m_dailymotionRequest, SIGNAL(finished()), this, SLOT(onDailymotionRequestFinished()));
    }
    
    return m_dailymotionRequest;
}

QVimeo::StreamsRequest* PluginStreamModel::vimeoRequest() {
    if (!m_vimeoRequest) {
        m_vimeoRequest = new QVimeo::StreamsRequest(this);
        connect(m_vimeoRequest, SIGNAL(finished()), this, SLOT(onVimeoRequestFinished()));
    }
    
    return m_vimeoRequest;
}

QYouTube::StreamsRequest* PluginStreamModel::youtubeRequest() {
    if (!m_youtubeRequest) {
        m_youtubeRequest = new QYouTube::StreamsRequest(this);
        connect(m_youtubeRequest, SIGNAL(finished()), this, SLOT(onYouTubeRequestFinished()));
    }
    
    return m_youtubeRequest;
}

void PluginStreamModel::onRequestFinished() {
    if (m_request->status() == ResourcesRequest::Ready) {
        const QVariantMap result = m_request->result().toMap();
        
        if (result.contains("items")) {
            foreach (const QVariant &v, result.value("items").toList()) {
                const QVariantMap stream = v.toMap();
                const int height = stream.value("height").toInt();
                
                if (height > 0) {
                    append(QString("%1p %2").arg(height).arg(stream.value("description").toString()), stream);
                }
                else {
                    append(stream.value("description").toString(), stream);
                }
            }
            
            setErrorString(QString());
            setStatus(ResourcesRequest::Ready);
        }
        else if (result.contains("service")) {
            const QString service = result.value("service").toString();
            
            if (service == Resources::DAILYMOTION) {
                dailymotionRequest()->list(result.value("id").toString());
            }
            else if (service == Resources::VIMEO) {
                vimeoRequest()->list(result.value("id").toString());
            }
            else if (service == Resources::YOUTUBE) {
                youtubeRequest()->list(result.value("id").toString());
            }
            else {
                setErrorString(tr("Attempted redirect to unsupported service '%1'").arg(service));
                setStatus(ResourcesRequest::Failed);
                Logger::log("PluginStreamModel::onRequestFinished(). Error: " + errorString());
            }
        }
        else {
            setErrorString(tr("Unknown error"));
            setStatus(ResourcesRequest::Failed);
            Logger::log("PluginStreamModel::onRequestFinished(). Error: " + errorString());
        }
    }
    else {
        setErrorString(m_request->errorString());
        setStatus(ResourcesRequest::Failed);
        Logger::log("PluginStreamModel::onRequestFinished(). Error: " + errorString());
    }    
}

void PluginStreamModel::onDailymotionRequestFinished() {
    if (m_dailymotionRequest->status() == QDailymotion::StreamsRequest::Ready) {
        foreach (const QVariant &v, m_dailymotionRequest->result().toList()) {
            const QVariantMap stream = v.toMap();
            append(QString("%1p %2").arg(stream.value("height").toInt()).arg(stream.value("description").toString()),
                   stream);
        }
        
        setErrorString(QString());
        setStatus(ResourcesRequest::Ready);
    }
    else {
        setErrorString(m_dailymotionRequest->errorString());
        setStatus(ResourcesRequest::Failed);
        Logger::log("PluginStreamModel::onDailymotionRequestFinished(). Error: " + errorString());
    }
    
}

void PluginStreamModel::onVimeoRequestFinished() {
    if (m_vimeoRequest->status() == QVimeo::StreamsRequest::Ready) {
        foreach (const QVariant &v, m_vimeoRequest->result().toList()) {
            const QVariantMap stream = v.toMap();
            append(QString("%1p %2").arg(stream.value("height").toInt()).arg(stream.value("description").toString()),
                   stream);
        }
        
        setErrorString(QString());
        setStatus(ResourcesRequest::Ready);
    }
    else {
        setErrorString(m_vimeoRequest->errorString());
        setStatus(ResourcesRequest::Failed);
        Logger::log("PluginStreamModel::onVimeoRequestFinished(). Error: " + errorString());
    }
}

void PluginStreamModel::onYouTubeRequestFinished() {
    if (m_youtubeRequest->status() == QYouTube::StreamsRequest::Ready) {
        foreach (const QVariant &v, m_youtubeRequest->result().toList()) {
            const QVariantMap stream = v.toMap();
            append(QString("%1p %2").arg(stream.value("height").toInt()).arg(stream.value("description").toString()),
                   stream);
        }
        
        setErrorString(QString());
        setStatus(ResourcesRequest::Ready);
    }
    else {
        setErrorString(m_youtubeRequest->errorString());
        setStatus(ResourcesRequest::Failed);
        Logger::log("PluginStreamModel::onYouTubeRequestFinished(). Error: " + errorString());
    }
}
