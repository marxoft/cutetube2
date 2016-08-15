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

#ifndef PLUGINTRANSFER_H
#define PLUGINTRANSFER_H

#include "transfer.h"

class ResourcesRequest;

namespace QDailymotion {
    class ResourcesRequest;
    class StreamsRequest;
}

namespace QVimeo {
    class ResourcesRequest;
    class StreamsRequest;
}

namespace QYouTube {
    class StreamsRequest;
    class SubtitlesRequest;
}

class PluginTransfer : public Transfer
{
    Q_OBJECT
    
public:
    explicit PluginTransfer(const QString &service, QObject *parent = 0);

private Q_SLOTS:
    void onStreamsRequestFinished();
    void onSubtitlesRequestFinished();
    void onDailymotionStreamsRequestFinished();
    void onDailymotionSubtitlesRequestFinished();
    void onVimeoStreamsRequestFinished();
    void onVimeoSubtitlesRequestFinished();
    void onYouTubeStreamsRequestFinished();
    void onYouTubeSubtitlesRequestFinished();

private:
    void listStreams();
    void listSubtitles();

    ResourcesRequest* streamsRequest();
    ResourcesRequest* subtitlesRequest();
    QDailymotion::StreamsRequest* dailymotionStreamsRequest();
    QDailymotion::ResourcesRequest* dailymotionSubtitlesRequest();
    QVimeo::StreamsRequest* vimeoStreamsRequest();
    QVimeo::ResourcesRequest* vimeoSubtitlesRequest();
    QYouTube::StreamsRequest* youtubeStreamsRequest();
    QYouTube::SubtitlesRequest* youtubeSubtitlesRequest();
    
    ResourcesRequest *m_streamsRequest;
    ResourcesRequest *m_subtitlesRequest;
    QDailymotion::StreamsRequest *m_dailymotionStreamsRequest;
    QDailymotion::ResourcesRequest *m_dailymotionSubtitlesRequest;
    QVimeo::StreamsRequest *m_vimeoStreamsRequest;
    QVimeo::ResourcesRequest *m_vimeoSubtitlesRequest;
    QYouTube::StreamsRequest *m_youtubeStreamsRequest;
    QYouTube::SubtitlesRequest *m_youtubeSubtitlesRequest;
};
    
#endif // PLUGINTRANSFER_H
