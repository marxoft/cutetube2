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
#include <qdailymotion/resourcesrequest.h>
#include <qdailymotion/streamsrequest.h>

DailymotionTransfer::DailymotionTransfer(QObject *parent) :
    Transfer(parent),
    m_streamsRequest(0),
    m_subtitlesRequest(0)
{
}

void DailymotionTransfer::listStreams() {
    if (!m_streamsRequest) {
        m_streamsRequest = new QDailymotion::StreamsRequest(this);
        connect(m_streamsRequest, SIGNAL(finished()), this, SLOT(onStreamsRequestFinished()));
    }
    
    m_streamsRequest->list(videoId());
}

void DailymotionTransfer::listSubtitles() {
    if (!m_subtitlesRequest) {
        m_subtitlesRequest = new QDailymotion::ResourcesRequest(this);
        connect(m_subtitlesRequest, SIGNAL(finished()), this, SLOT(onSubtitlesRequestFinished()));
    }
    
    m_subtitlesRequest->list(QString("/video/%1/subtitles").arg(videoId()));
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
    
    if (!executeCustomCommands()) {
        moveDownloadedFiles();
    }
}
