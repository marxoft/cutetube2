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

#include "plugintransfer.h"
#include "dailymotion.h"
#include "logger.h"
#include "pluginmanager.h"
#include "resources.h"
#include "resourcesrequest.h"
#include "vimeo.h"
#include <qdailymotion/resourcesrequest.h>
#include <qdailymotion/streamsrequest.h>
#include <qvimeo/resourcesrequest.h>
#include <qvimeo/streamsrequest.h>
#include <qyoutube/streamsrequest.h>
#include <qyoutube/subtitlesrequest.h>
#if QT_VERSION >= 0x050000
#include <QUrlQuery>
#endif

PluginTransfer::PluginTransfer(const QString &service, QObject *parent) :
    Transfer(parent),
    m_streamsRequest(0),
    m_subtitlesRequest(0),
    m_dailymotionStreamsRequest(0),
    m_dailymotionSubtitlesRequest(0),
    m_vimeoStreamsRequest(0),
    m_vimeoSubtitlesRequest(0),
    m_youtubeStreamsRequest(0),
    m_youtubeSubtitlesRequest(0)
{
    setService(service);
}

void PluginTransfer::listStreams() {
    if (ResourcesRequest *r = streamsRequest()) {
        Logger::log("PluginTransfer::listStreams(). ID: " + videoId(), Logger::MediumVerbosity);
        r->list(Resources::STREAM, videoId());
    }
    else {
        setErrorString(tr("No streams plugin found for service '%1'").arg(service()));
        Logger::log("PluginTransfer::listStreams(). Error: " + errorString());
        setStatus(Failed);
    }
}

void PluginTransfer::listSubtitles() {
    if (ResourcesRequest *r = subtitlesRequest()) {
        r->list(Resources::SUBTITLE, videoId());
    }
    else {
        setErrorString(tr("No subtitles plugin found for service '%1'").arg(service()));
        Logger::log("PluginTransfer::listSubtitles(). Error: " + errorString());
        setStatus(Failed);
    }
}

ResourcesRequest* PluginTransfer::streamsRequest() {
    if (!m_streamsRequest) {
        m_streamsRequest = PluginManager::instance()->createRequestForService(service(), this);

        if (m_streamsRequest) {
            connect(m_streamsRequest, SIGNAL(finished()), this, SLOT(onStreamsRequestFinished()));
        }
    }

    return m_streamsRequest;
}

ResourcesRequest* PluginTransfer::subtitlesRequest() {
    if (!m_subtitlesRequest) {
        m_subtitlesRequest = PluginManager::instance()->createRequestForService(service(), this);

        if (m_subtitlesRequest) {
            connect(m_subtitlesRequest, SIGNAL(finished()), this, SLOT(onSubtitlesRequestFinished()));
        }
    }

    return m_subtitlesRequest;
}

QDailymotion::StreamsRequest* PluginTransfer::dailymotionStreamsRequest() {
    if (!m_dailymotionStreamsRequest) {
        m_dailymotionStreamsRequest = new QDailymotion::StreamsRequest(this);
        connect(m_dailymotionStreamsRequest, SIGNAL(finished()), this, SLOT(onDailymotionStreamsRequestFinished()));
    }
    
    return m_dailymotionStreamsRequest;
}

QDailymotion::ResourcesRequest* PluginTransfer::dailymotionSubtitlesRequest() {
    if (!m_dailymotionSubtitlesRequest) {
        m_dailymotionSubtitlesRequest = new QDailymotion::ResourcesRequest(this);
        m_dailymotionSubtitlesRequest->setClientId(Dailymotion::clientId());
        m_dailymotionSubtitlesRequest->setClientSecret(Dailymotion::clientSecret());
        m_dailymotionSubtitlesRequest->setAccessToken(Dailymotion::accessToken());
        m_dailymotionSubtitlesRequest->setRefreshToken(Dailymotion::refreshToken());
        
        connect(m_dailymotionSubtitlesRequest, SIGNAL(accessTokenChanged(QString)),
                Dailymotion::instance(), SLOT(setAccessToken(QString)));
        connect(m_dailymotionSubtitlesRequest, SIGNAL(refreshTokenChanged(QString)),
                Dailymotion::instance(), SLOT(setRefreshToken(QString)));
        connect(m_dailymotionSubtitlesRequest, SIGNAL(finished()), this, SLOT(onDailymotionSubtitlesRequestFinished()));
    }
    
    return m_dailymotionSubtitlesRequest;
}

QVimeo::StreamsRequest* PluginTransfer::vimeoStreamsRequest() {
    if (!m_vimeoStreamsRequest) {
        m_vimeoStreamsRequest = new QVimeo::StreamsRequest(this);
        connect(m_vimeoStreamsRequest, SIGNAL(finished()), this, SLOT(onVimeoStreamsRequestFinished()));
    }
    
    return m_vimeoStreamsRequest;
}

QVimeo::ResourcesRequest* PluginTransfer::vimeoSubtitlesRequest() {
    if (!m_vimeoSubtitlesRequest) {
        m_vimeoSubtitlesRequest = new QVimeo::ResourcesRequest(this);
        m_vimeoSubtitlesRequest->setClientId(Vimeo::clientId());
        m_vimeoSubtitlesRequest->setClientSecret(Vimeo::clientSecret());
        m_vimeoSubtitlesRequest->setAccessToken(Vimeo::accessToken());
        
        connect(m_vimeoSubtitlesRequest, SIGNAL(accessTokenChanged(QString)),
                Vimeo::instance(), SLOT(setAccessToken(QString)));
        connect(m_vimeoSubtitlesRequest, SIGNAL(finished()), this, SLOT(onVimeoSubtitlesRequestFinished()));
    }
    
    return m_vimeoSubtitlesRequest;
}

QYouTube::StreamsRequest* PluginTransfer::youtubeStreamsRequest() {
    if (!m_youtubeStreamsRequest) {
        m_youtubeStreamsRequest = new QYouTube::StreamsRequest(this);
        connect(m_youtubeStreamsRequest, SIGNAL(finished()), this, SLOT(onYouTubeStreamsRequestFinished()));
    }
    
    return m_youtubeStreamsRequest;
}

QYouTube::SubtitlesRequest* PluginTransfer::youtubeSubtitlesRequest() {
    if (!m_youtubeSubtitlesRequest) {
        m_youtubeSubtitlesRequest = new QYouTube::SubtitlesRequest(this);
        connect(m_youtubeSubtitlesRequest, SIGNAL(finished()), this, SLOT(onYouTubeSubtitlesRequestFinished()));
    }
    
    return m_youtubeSubtitlesRequest;
}

void PluginTransfer::onStreamsRequestFinished() {
    if (m_streamsRequest->status() == ResourcesRequest::Ready) {
        const QVariantMap result = m_streamsRequest->result().toMap();
        
        if (result.contains("items")) {
            const QVariantList list = result.value("items").toList();
            
            foreach (const QVariant &v, list) {
                const QVariantMap stream = v.toMap();
                
                if (stream.value("id") == streamId()) {
                    startDownload(stream.value("url").toString());
                    return;
                }
            }
        }
        else if (result.contains("service")) {
            const QString service = result.value("service").toString();
            
            if (service == Resources::DAILYMOTION) {
                dailymotionStreamsRequest()->list(result.value("id").toString());
            }
            else if (service == Resources::VIMEO) {
                vimeoStreamsRequest()->list(QString("/videos/%1/texttracks").arg(result.value("id").toString()));
            }
            else if (service == Resources::YOUTUBE) {
                youtubeStreamsRequest()->list(result.value("id").toString());
            }
            else {
                setErrorString(tr("Attempted redirect to unsupported service '%1'").arg(service));
                Logger::log("PluginTransfer::onStreamsRequestFinished(). Error: " + errorString());
                setStatus(Failed);
            }
            
            return;
        }
    }
    
    setErrorString(tr("No stream URL found"));
    Logger::log("PluginTransfer::onStreamsRequestFinished(). Error: " + errorString());
    setStatus(Failed);
}

void PluginTransfer::onSubtitlesRequestFinished() {
    if (m_subtitlesRequest->status() == ResourcesRequest::Ready) {
        const QVariantMap result = m_subtitlesRequest->result().toMap();
        
        if (result.contains("items")) {
            const QVariantList list = result.value("items").toList();
            
            foreach (const QVariant &v, list) {
                const QVariantMap sub = v.toMap();
                
                if (sub.value("title") == subtitlesLanguage()) {
                    startSubtitlesDownload(sub.value("url").toString());
                    return;
                }
            }
        }
        else if (result.contains("service")) {
            const QString service = result.value("service").toString();
            
            if (service == Resources::DAILYMOTION) {
                dailymotionSubtitlesRequest()->list(QString("/video/%1/subtitles").arg(result.value("id").toString()),
                                                    QVariantMap(), Dailymotion::SUBTITLE_FIELDS);
                return;
            }
            
            if (service == Resources::VIMEO) {
                vimeoSubtitlesRequest()->list(result.value("id").toString());
                return;
            }
            
            if (service == Resources::YOUTUBE) {
                youtubeSubtitlesRequest()->list(result.value("id").toString());
                return;
            }
        }
    }
    else {
        Logger::log("PluginTransfer::onSubtitlesRequestFinished(). Error: " + m_subtitlesRequest->errorString());
    }
    
    if (!executeCustomCommands()) {
        moveDownloadedFiles();
    }
}

void PluginTransfer::onDailymotionStreamsRequestFinished() {
    if (m_dailymotionStreamsRequest->status() == QDailymotion::StreamsRequest::Ready) {
        const QVariantList list = m_dailymotionStreamsRequest->result().toList();
        
        foreach (const QVariant &v, list) {
            const QVariantMap stream = v.toMap();
        
            if (stream.value("id") == streamId()) {
                startDownload(stream.value("url").toString());
                return;
            }
        }
    }
    
    setErrorString(tr("No stream URL found"));
    Logger::log("PluginTransfer::onDailymotionStreamsRequestFinished(). Error: " + errorString());
    setStatus(Failed);
}

void PluginTransfer::onDailymotionSubtitlesRequestFinished() {
    if (m_dailymotionSubtitlesRequest->status() == QDailymotion::ResourcesRequest::Ready) {
        const QVariantList list = m_dailymotionSubtitlesRequest->result().toMap().value("list").toList();
    
        foreach (const QVariant &v, list) {
            const QVariantMap sub = v.toMap();
        
            if (sub.value("name") == subtitlesLanguage()) {
                startSubtitlesDownload(sub.value("url").toString());
                return;
            }
        }
    }
    else {
        Logger::log("PluginTransfer::onDailymotionSubtitlesRequestFinished(). Error: "
                    + Dailymotion::getErrorString(m_dailymotionSubtitlesRequest->result().toMap()));
    }
    
    if (!executeCustomCommands()) {
        moveDownloadedFiles();
    }
}

void PluginTransfer::onVimeoStreamsRequestFinished() {
    if (m_vimeoStreamsRequest->status() == QVimeo::StreamsRequest::Ready) {
        const QVariantList list = m_vimeoStreamsRequest->result().toList();
        
        foreach (const QVariant &v, list) {
            const QVariantMap stream = v.toMap();
        
            if (stream.value("id") == streamId()) {
                startDownload(stream.value("url").toString());
                return;
            }
        }
    }
    
    setErrorString(tr("No stream URL found"));
    Logger::log("PluginTransfer::onVimeoStreamsRequestFinished(). Error: " + errorString());
    setStatus(Failed);
}

void PluginTransfer::onVimeoSubtitlesRequestFinished() {
    if (m_vimeoSubtitlesRequest->status() == QVimeo::ResourcesRequest::Ready) {
        const QVariantList list = m_vimeoSubtitlesRequest->result().toMap().value("items").toList();
    
        foreach (const QVariant &v, list) {
            const QVariantMap sub = v.toMap();
        
            if (sub.value("name") == subtitlesLanguage()) {
                startSubtitlesDownload(sub.value("url").toString());
                return;
            }
        }
    }
    else {
        Logger::log("PluginTransfer::onVimeoSubtitlessRequestFinished(). Error: "
                    + Vimeo::getErrorString(m_vimeoSubtitlesRequest->result().toMap()));
    }
    
    if (!executeCustomCommands()) {
        moveDownloadedFiles();
    }
}

void PluginTransfer::onYouTubeStreamsRequestFinished() {
    if (m_youtubeStreamsRequest->status() == QYouTube::StreamsRequest::Ready) {
        const QVariantList list = m_youtubeStreamsRequest->result().toList();
        
        foreach (const QVariant &v, list) {
            const QVariantMap stream = v.toMap();
        
            if (stream.value("id") == streamId()) {
                startDownload(stream.value("url").toString());
                return;
            }
        }
    }
    
    setErrorString(tr("No stream URL found"));
    Logger::log("PluginTransfer::onYouTubeStreamsRequestFinished(). Error: " + errorString());
    setStatus(Failed);
}

void PluginTransfer::onYouTubeSubtitlesRequestFinished() {
    if (m_youtubeSubtitlesRequest->status() == QYouTube::SubtitlesRequest::Ready) {
        const QVariantList list = m_youtubeSubtitlesRequest->result().toList();
    
        foreach (const QVariant &v, list) {
            const QVariantMap sub = v.toMap();
        
            if (sub.value("translatedLanguage") == subtitlesLanguage()) {
                QUrl u(sub.value("url").toString());
#if QT_VERSION >= 0x050000
                QUrlQuery query(u);
                query.addQueryItem("fmt", "srt");
                u.setQuery(query);
#else
                u.addQueryItem("fmt", "srt");
#endif
                startSubtitlesDownload(u);
                return;
            }
        }
    }
    else {
        Logger::log("PluginTransfer::onYouTubeSubtitlesRequestFinished(). Error: "
                    + m_youtubeSubtitlesRequest->errorString());
    }
    
    if (!executeCustomCommands()) {
        moveDownloadedFiles();
    }
}
