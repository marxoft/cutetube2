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

#include "dailymotiontransfer.h"
#include <qdailymotion/resourcesrequest.h>
#include <qdailymotion/streamsrequest.h>
#ifdef CUTETUBE_DEBUG
#include <QDebug>
#endif

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
    
    m_streamsRequest->list(resourceId());
}

void DailymotionTransfer::listSubtitles() {
    if (!m_subtitlesRequest) {
        m_subtitlesRequest = new QDailymotion::ResourcesRequest(this);
        connect(m_subtitlesRequest, SIGNAL(finished()), this, SLOT(onSubtitlesRequestFinished()));
    }
    
    m_subtitlesRequest->list(QString("/video/%1/subtitles").arg(resourceId()));
}

void DailymotionTransfer::onStreamsRequestFinished() {
    if (m_streamsRequest->status() == QDailymotion::StreamsRequest::Ready) {
        QVariantList list = m_streamsRequest->result().toList();
        
        foreach (QVariant v, list) {
            QVariantMap stream = v.toMap();
        
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
        QVariantList list = m_subtitlesRequest->result().toMap().value("list").toList();
    
        foreach (QVariant v, list) {
            QVariantMap sub = v.toMap();
        
            if (sub.value("name") == subtitlesLanguage()) {
                startSubtitlesDownload(sub.value("url").toString());
                return;
            }
        }
    }
#ifdef CUTETUBE_DEBUG
    qDebug() << "DailymotionTransfer::onSubtitlesRequestFinished: No subtitles found";
#endif
    if (convertToAudio()) {
        startAudioConversion();
    }
    else {
        moveDownloadedFiles();
    }
}
