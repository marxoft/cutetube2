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

#include "youtubetransfer.h"
#include <qyoutube/streamsrequest.h>
#include <qyoutube/subtitlesrequest.h>
#if QT_VERSION >= 0x050000
#include <QUrlQuery>
#endif
#ifdef CUTETUBE_DEBUG
#include <QDebug>
#endif

YouTubeTransfer::YouTubeTransfer(QObject *parent) :
    Transfer(parent),
    m_streamsRequest(0),
    m_subtitlesRequest(0)
{
}

void YouTubeTransfer::listStreams() {
    if (!m_streamsRequest) {
        m_streamsRequest = new QYouTube::StreamsRequest(this);
        connect(m_streamsRequest, SIGNAL(finished()), this, SLOT(onStreamsRequestFinished()));
    }
    
    m_streamsRequest->list(resourceId());
}

void YouTubeTransfer::listSubtitles() {
    if (!m_subtitlesRequest) {
        m_subtitlesRequest = new QYouTube::SubtitlesRequest(this);
        connect(m_subtitlesRequest, SIGNAL(finished()), this, SLOT(onSubtitlesRequestFinished()));
    }
    
    m_subtitlesRequest->list(resourceId());
}

void YouTubeTransfer::onStreamsRequestFinished() {
    if (m_streamsRequest->status() == QYouTube::StreamsRequest::Ready) {
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

void YouTubeTransfer::onSubtitlesRequestFinished() {
    if (m_subtitlesRequest->status() == QYouTube::SubtitlesRequest::Ready) {
        QVariantList list = m_subtitlesRequest->result().toList();
    
        foreach (QVariant v, list) {
            QVariantMap sub = v.toMap();
        
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
#ifdef CUTETUBE_DEBUG
    qDebug() << "YouTubeTransfer::onSubtitlesRequestFinished: No subtitles found";
#endif
    if (convertToAudio()) {
        startAudioConversion();
    }
    else {
        moveDownloadedFiles();
    }
}
