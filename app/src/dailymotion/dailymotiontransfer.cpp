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

#include "dailymotiontransfer.h"
#include "dailymotion.h"
#include "logger.h"
#include "resources.h"
#include <qdailymotion/resourcesrequest.h>
#include <qdailymotion/streamsrequest.h>

DailymotionTransfer::DailymotionTransfer(QObject *parent) :
    Transfer(parent),
    m_streamsRequest(0),
    m_subtitlesRequest(0)
{
    setService(Resources::DAILYMOTION);
}

void DailymotionTransfer::listStreams() {
    Logger::log("DailymotionTransfer::listStreams(). ID: " + videoId(), Logger::MediumVerbosity);
    streamsRequest()->list(videoId());
}

void DailymotionTransfer::listSubtitles() {
    Logger::log("DailymotionTransfer::listSubtitles(). ID: " + videoId(), Logger::MediumVerbosity);
    subtitlesRequest()->list(QString("/video/%1/subtitles").arg(videoId()), QVariantMap(),
                             Dailymotion::SUBTITLE_FIELDS);
}

QDailymotion::StreamsRequest* DailymotionTransfer::streamsRequest() {
    if (!m_streamsRequest) {
        m_streamsRequest = new QDailymotion::StreamsRequest(this);
        connect(m_streamsRequest, SIGNAL(finished()), this, SLOT(onStreamsRequestFinished()));
    }
    
    return m_streamsRequest;
}

QDailymotion::ResourcesRequest* DailymotionTransfer::subtitlesRequest() {
    if (!m_subtitlesRequest) {
        m_subtitlesRequest = new QDailymotion::ResourcesRequest(this);
        m_subtitlesRequest->setClientId(Dailymotion::clientId());
        m_subtitlesRequest->setClientSecret(Dailymotion::clientSecret());
        m_subtitlesRequest->setAccessToken(Dailymotion::accessToken());
        m_subtitlesRequest->setRefreshToken(Dailymotion::refreshToken());
        
        connect(m_subtitlesRequest, SIGNAL(accessTokenChanged(QString)),
                Dailymotion::instance(), SLOT(setAccessToken(QString)));
        connect(m_subtitlesRequest, SIGNAL(refreshTokenChanged(QString)),
                Dailymotion::instance(), SLOT(setRefreshToken(QString)));
        connect(m_subtitlesRequest, SIGNAL(finished()), this, SLOT(onSubtitlesRequestFinished()));
    }
    
    return m_subtitlesRequest;
}

void DailymotionTransfer::onStreamsRequestFinished() {
    if (m_streamsRequest->status() == QDailymotion::StreamsRequest::Ready) {
        const QVariantList list = m_streamsRequest->result().toList();
        
        foreach (const QVariant &v, list) {
            const QVariantMap stream = v.toMap();
        
            if (stream.value("id") == streamId()) {
                startDownload(stream.value("url").toString());
                return;
            }
        }
    }
    else {
        Logger::log("DailymotionTransfer::onStreamsRequestFinished(). Error: " + m_streamsRequest->errorString());
    }
    
    setErrorString(tr("No stream URL found"));
    setStatus(Failed);
}

void DailymotionTransfer::onSubtitlesRequestFinished() {
    if (m_subtitlesRequest->status() == QDailymotion::ResourcesRequest::Ready) {
        const QVariantList list = m_subtitlesRequest->result().toMap().value("list").toList();
    
        foreach (const QVariant &v, list) {
            const QVariantMap sub = v.toMap();
        
            if (sub.value("name") == subtitlesLanguage()) {
                startSubtitlesDownload(sub.value("url").toString());
                return;
            }
        }
    }
    else {
        Logger::log("DailymotionTransfer::onSubtitlesRequestFinished(). Error: "
                    + Dailymotion::getErrorString(m_subtitlesRequest->result().toMap()));
    }
    
    if (!executeCustomCommands()) {
        moveDownloadedFiles();
    }
}
