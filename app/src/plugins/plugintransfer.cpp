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

#include "plugintransfer.h"
#include "resources.h"
#include "resourcesrequest.h"
#ifdef CUTETUBE_DEBUG
#include <QDebug>
#endif

PluginTransfer::PluginTransfer(const QString &service, QObject *parent) :
    Transfer(parent),
    m_streamsRequest(0),
    m_subtitlesRequest(0)
{
    setService(service);
}

void PluginTransfer::listStreams() {
    if (!m_streamsRequest) {
        m_streamsRequest = new ResourcesRequest(this);
        connect(m_streamsRequest, SIGNAL(finished()), this, SLOT(onStreamsRequestFinished()));
    }
    
    m_streamsRequest->list(Resources::STREAM, resourceId());
}

void PluginTransfer::listSubtitles() {
    if (!m_subtitlesRequest) {
        m_subtitlesRequest = new ResourcesRequest(this);
        connect(m_subtitlesRequest, SIGNAL(finished()), this, SLOT(onSubtitlesRequestFinished()));
    }
    
    m_subtitlesRequest->list(Resources::SUBTITLE, resourceId());
}

void PluginTransfer::onStreamsRequestFinished() {
    if (m_streamsRequest->status() == ResourcesRequest::Ready) {
        QVariantList list = m_streamsRequest->result().toMap().value("items").toList();
        
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

void PluginTransfer::onSubtitlesRequestFinished() {
    if (m_subtitlesRequest->status() == ResourcesRequest::Ready) {
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
    qDebug() << "PluginTransfer::onSubtitlesRequestFinished: No subtitles found";
#endif
    if (convertToAudio()) {
        startAudioConversion();
    }
    else {
        moveDownloadedFiles();
    }
}
