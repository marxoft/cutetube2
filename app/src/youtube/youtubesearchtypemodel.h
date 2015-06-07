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

#ifndef YOUTUBESEARCHTYPEMODEL_H
#define YOUTUBESEARCHTYPEMODEL_H

#include "selectionmodel.h"
#include "resources.h"

class YouTubeSearchTypeModel : public SelectionModel
{
    Q_OBJECT

public:
    explicit YouTubeSearchTypeModel(QObject *parent = 0) :
        SelectionModel(parent)
    {
        append(tr("Videos (date)"), SearchResource(tr("Videos (date)"), Resources::VIDEO, "date"));
        append(tr("Videos (relevance)"), SearchResource(tr("Videos (relevance)"), Resources::VIDEO, "relevance"));
        append(tr("Videos (popular)"), SearchResource(tr("Videos (popular)"), Resources::VIDEO, "viewCount"));
        append(tr("Videos (rating)"), SearchResource(tr("Videos (rating)"), Resources::VIDEO, "rating"));
        append(tr("Videos (title)"), SearchResource(tr("Videos (title)"), Resources::VIDEO, "title"));
        append(tr("Playlists (date)"), SearchResource(tr("Playlists (date)"), Resources::PLAYLIST, "date"));
        append(tr("Playlists (relevance)"), SearchResource(tr("Playlists (relevance)"), Resources::PLAYLIST, "relevance"));
        append(tr("Playlists (popular)"), SearchResource(tr("Playlists (popular)"), Resources::PLAYLIST, "viewCount"));
        append(tr("Playlists (rating)"), SearchResource(tr("Playlists (rating)"), Resources::PLAYLIST, "rating"));
        append(tr("Playlists (title)"), SearchResource(tr("Playlists (title)"), Resources::PLAYLIST, "title"));
        append(tr("Channels (date)"), SearchResource(tr("Channels (date)"), Resources::USER, "date"));
        append(tr("Channels (relevance)"), SearchResource(tr("Channels (relevance)"), Resources::USER, "relevance"));
        append(tr("Channels (popular)"), SearchResource(tr("Channels (popular)"), Resources::USER, "viewCount"));
        append(tr("Channels (rating)"), SearchResource(tr("Channels (rating)"), Resources::USER, "rating"));
        append(tr("Channels (title)"), SearchResource(tr("Channels (title)"), Resources::USER, "title"));
        append(tr("Channels (video count)"), SearchResource(tr("Channels (video count)"), Resources::USER, "videoCount"));
        
    }
};

#endif // YOUTUBESEARCHTYPEMODEL_H
