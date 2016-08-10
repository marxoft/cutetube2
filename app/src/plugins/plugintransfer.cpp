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
#include "pluginmanager.h"
#include "resources.h"
#include "resourcesrequest.h"

PluginTransfer::PluginTransfer(const QString &service, QObject *parent) :
    Transfer(parent),
    m_streamsRequest(0),
    m_subtitlesRequest(0)
{
    setService(service);
}

void PluginTransfer::listStreams() {
    if (ResourcesRequest *r = streamsRequest()) {
        r->list(Resources::STREAM, videoId());
    }
}

void PluginTransfer::listSubtitles() {
    if (ResourcesRequest *r = subtitlesRequest()) {
        r->list(Resources::SUBTITLE, videoId());
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

void PluginTransfer::onStreamsRequestFinished() {
    if (m_streamsRequest->status() == ResourcesRequest::Ready) {
        const QVariantList list = m_streamsRequest->result().toMap().value("items").toList();
        
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

void PluginTransfer::onSubtitlesRequestFinished() {
    if (m_subtitlesRequest->status() == ResourcesRequest::Ready) {
        const QVariantList list = m_subtitlesRequest->result().toMap().value("items").toList();
    
        foreach (const QVariant &v, list) {
            const QVariantMap sub = v.toMap();
        
            if (sub.value("title") == subtitlesLanguage()) {
                startSubtitlesDownload(sub.value("url").toString());
                return;
            }
        }
    }
    
    if (!executeCustomCommands()) {
        moveDownloadedFiles();
    }
}
