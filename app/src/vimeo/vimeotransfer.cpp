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

#include "vimeotransfer.h"
#include "logger.h"
#include "resources.h"
#include "vimeo.h"
#include <qvimeo/resourcesrequest.h>
#include <qvimeo/streamsrequest.h>

VimeoTransfer::VimeoTransfer(QObject *parent) :
    Transfer(parent),
    m_streamsRequest(0),
    m_subtitlesRequest(0)
{
    setService(Resources::VIMEO);
}

void VimeoTransfer::listStreams() {
    Logger::log("VimeoTransfer::listStreams(). ID: " + videoId(), Logger::MediumVerbosity);
    streamsRequest()->list(videoId());
}

void VimeoTransfer::listSubtitles() {
    Logger::log("VimeoTransfer::listSubtitles(). ID: " + videoId(), Logger::MediumVerbosity);
    subtitlesRequest()->list(QString("/videos/%1/texttracks").arg(videoId()));
}

QVimeo::StreamsRequest* VimeoTransfer::streamsRequest() {
    if (!m_streamsRequest) {
        m_streamsRequest = new QVimeo::StreamsRequest(this);
        connect(m_streamsRequest, SIGNAL(finished()), this, SLOT(onStreamsRequestFinished()));
    }
    
    return m_streamsRequest;
}

QVimeo::ResourcesRequest* VimeoTransfer::subtitlesRequest() {
    if (!m_subtitlesRequest) {
        m_subtitlesRequest = new QVimeo::ResourcesRequest(this);
        m_subtitlesRequest->setClientId(Vimeo::clientId());
        m_subtitlesRequest->setClientSecret(Vimeo::clientSecret());
        m_subtitlesRequest->setAccessToken(Vimeo::accessToken());
        
        connect(m_subtitlesRequest, SIGNAL(accessTokenChanged(QString)),
                Vimeo::instance(), SLOT(setAccessToken(QString)));
        connect(m_subtitlesRequest, SIGNAL(finished()), this, SLOT(onSubtitlesRequestFinished()));
    }
    
    return m_subtitlesRequest;
}

void VimeoTransfer::onStreamsRequestFinished() {
    if (m_streamsRequest->status() == QVimeo::StreamsRequest::Ready) {
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
    Logger::log("VimeoTransfer::onStreamsRequestFinished(). Error: " + errorString());
    setStatus(Failed);
}

void VimeoTransfer::onSubtitlesRequestFinished() {
    if (m_subtitlesRequest->status() == QVimeo::ResourcesRequest::Ready) {
        const QVariantList list = m_subtitlesRequest->result().toMap().value("items").toList();
    
        foreach (const QVariant &v, list) {
            const QVariantMap sub = v.toMap();
        
            if (sub.value("name") == subtitlesLanguage()) {
                startSubtitlesDownload(sub.value("url").toString());
                return;
            }
        }
    }
    else {
        Logger::log("VimeoTransfer::onSubtitlesRequestFinished(). Error: "
                    + Vimeo::getErrorString(m_subtitlesRequest->result().toMap()));
    }
    
    if (!executeCustomCommands()) {
        moveDownloadedFiles();
    }
}
