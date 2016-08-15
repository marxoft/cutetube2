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

#ifndef DAILYMOTIONTRANSFER_H
#define DAILYMOTIONTRANSFER_H

#include "transfer.h"

namespace QDailymotion {
    class ResourcesRequest;
    class StreamsRequest;
}

class DailymotionTransfer : public Transfer
{
    Q_OBJECT
    
public:
    explicit DailymotionTransfer(QObject *parent = 0);
    
private:
    void listStreams();
    void listSubtitles();

private Q_SLOTS:
    void onStreamsRequestFinished();
    void onSubtitlesRequestFinished();

private:
    QDailymotion::StreamsRequest* streamsRequest();
    QDailymotion::ResourcesRequest* subtitlesRequest();
    
    QDailymotion::StreamsRequest *m_streamsRequest;
    QDailymotion::ResourcesRequest *m_subtitlesRequest;
};
    
#endif // DAILYMOTIONTRANSFER_H
