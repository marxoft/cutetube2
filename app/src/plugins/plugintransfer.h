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

class PluginTransfer : public Transfer
{
    Q_OBJECT
    
public:
    explicit PluginTransfer(const QString &service, QObject *parent = 0);

private Q_SLOTS:
    void onStreamsRequestFinished();
    void onSubtitlesRequestFinished();

private:
    void listStreams();
    void listSubtitles();

    ResourcesRequest* streamsRequest();
    ResourcesRequest* subtitlesRequest();
    
    ResourcesRequest *m_streamsRequest;
    ResourcesRequest *m_subtitlesRequest;
};
    
#endif // PLUGINTRANSFER_H
