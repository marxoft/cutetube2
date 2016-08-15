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

#include "youtubetransfer.h"
#include "logger.h"
#include "resources.h"
#include <qyoutube/streamsrequest.h>
#include <qyoutube/subtitlesrequest.h>
#if QT_VERSION >= 0x050000
#include <QUrlQuery>
#endif

YouTubeTransfer::YouTubeTransfer(QObject *parent) :
    Transfer(parent),
    m_streamsRequest(0),
    m_subtitlesRequest(0)
{
    setService(Resources::YOUTUBE);
}

void YouTubeTransfer::listStreams() {
    Logger::log("YouTubeTransfer::listStreams(). ID: " + videoId(), Logger::MediumVerbosity);
    streamsRequest()->list(videoId());
}

void YouTubeTransfer::listSubtitles() {
    Logger::log("YouTubeTransfer::listSubtitles(). ID: " + videoId(), Logger::MediumVerbosity);
    subtitlesRequest()->list(videoId());
}

QYouTube::StreamsRequest* YouTubeTransfer::streamsRequest() {
    if (!m_streamsRequest) {
        m_streamsRequest = new QYouTube::StreamsRequest(this);
        connect(m_streamsRequest, SIGNAL(finished()), this, SLOT(onStreamsRequestFinished()));
    }
    
    return m_streamsRequest;
}

QYouTube::SubtitlesRequest* YouTubeTransfer::subtitlesRequest() {
    if (!m_subtitlesRequest) {
        m_subtitlesRequest = new QYouTube::SubtitlesRequest(this);
        connect(m_subtitlesRequest, SIGNAL(finished()), this, SLOT(onSubtitlesRequestFinished()));
    }
    
    return m_subtitlesRequest;
}

void YouTubeTransfer::onStreamsRequestFinished() {
    if (m_streamsRequest->status() == QYouTube::StreamsRequest::Ready) {
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
    Logger::log("YouTubeTransfer::onStreamsRequestFinished(). Error: " + errorString());
    setStatus(Failed);
}

void YouTubeTransfer::onSubtitlesRequestFinished() {
    if (m_subtitlesRequest->status() == QYouTube::SubtitlesRequest::Ready) {
        const QVariantList list = m_subtitlesRequest->result().toList();
    
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
        Logger::log("YouTubeTransfer::onSubtitlesRequestFinished(). Error: " + m_subtitlesRequest->errorString());
    }
    
    if (!executeCustomCommands()) {
        moveDownloadedFiles();
    }
}
