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

#include "vimeotransfer.h"
#include "vimeo.h"
#include <qvimeo/resourcesrequest.h>
#include <qvimeo/streamsrequest.h>
#ifdef CUTETUBE_DEBUG
#include <QDebug>
#endif

VimeoTransfer::VimeoTransfer(QObject *parent) :
    Transfer(parent),
    m_streamsRequest(0),
    m_subtitlesRequest(0)
{
}

void VimeoTransfer::listStreams() {
    if (!m_streamsRequest) {
        m_streamsRequest = new QVimeo::StreamsRequest(this);
        connect(m_streamsRequest, SIGNAL(finished()), this, SLOT(onStreamsRequestFinished()));
    }
    
    m_streamsRequest->list(resourceId());
}

void VimeoTransfer::listSubtitles() {
    if (!m_subtitlesRequest) {
        m_subtitlesRequest = new QVimeo::ResourcesRequest(this);
        m_subtitlesRequest->setClientId(Vimeo::instance()->clientId());
        m_subtitlesRequest->setClientSecret(Vimeo::instance()->clientSecret());
        m_subtitlesRequest->setAccessToken(Vimeo::instance()->accessToken());
        connect(m_subtitlesRequest, SIGNAL(finished()), this, SLOT(onSubtitlesRequestFinished()));
    }
    
    m_subtitlesRequest->list(QString("/videos/%1/texttracks").arg(resourceId()));
}

void VimeoTransfer::onStreamsRequestFinished() {
    if (m_streamsRequest->status() == QVimeo::StreamsRequest::Ready) {
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

void VimeoTransfer::onSubtitlesRequestFinished() {
    if (m_subtitlesRequest->status() == QVimeo::ResourcesRequest::Ready) {
        QVariantList list = m_subtitlesRequest->result().toMap().value("items").toList();
    
        foreach (QVariant v, list) {
            QVariantMap sub = v.toMap();
        
            if (sub.value("name") == subtitlesLanguage()) {
                startSubtitlesDownload(sub.value("url").toString());
                return;
            }
        }
    }
#ifdef CUTETUBE_DEBUG
    qDebug() << "VimeoTransfer::onSubtitlesRequestFinished: No subtitles found";
#endif
    if (convertToAudio()) {
        startAudioConversion();
    }
    else {
        moveDownloadedFiles();
    }
}
