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
        append(tr("Videos"), Resources::VIDEO);
        append(tr("Playlists"), Resources::PLAYLIST);
        append(tr("Channels"), Resources::USER);
    }
};

#endif // YOUTUBESEARCHTYPEMODEL_H
