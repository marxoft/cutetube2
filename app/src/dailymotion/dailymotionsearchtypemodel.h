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

#ifndef DAILYMOTIONSEARCHTYPEMODEL_H
#define DAILYMOTIONSEARCHTYPEMODEL_H

#include "selectionmodel.h"
#include "resources.h"

class DailymotionSearchTypeModel : public SelectionModel
{
    Q_OBJECT

public:
    explicit DailymotionSearchTypeModel(QObject *parent = 0) :
        SelectionModel(parent)
    {
        append(tr("Videos (date)"), SearchResource(tr("Videos (date)"), Resources::VIDEO, "recent"));
        append(tr("Videos (relevance)"), SearchResource(tr("Videos (relevance)"), Resources::VIDEO, "relevance"));
        append(tr("Videos (views)"), SearchResource(tr("Videos (views)"), Resources::VIDEO, "visited"));
        append(tr("Playlists (date)"), SearchResource(tr("Playlists (date)"), Resources::PLAYLIST, "recent"));
        append(tr("Playlists (relevance)"), SearchResource(tr("Playlists (relevance)"), Resources::PLAYLIST, "relevance"));
        append(tr("Playlists (title)"), SearchResource(tr("Playlists (title)"), Resources::PLAYLIST, "alpha"));
        append(tr("Users (date)"), SearchResource(tr("Users (date)"), Resources::USER, "recent"));
        append(tr("Users (relevance)"), SearchResource(tr("Users (relevance)"), Resources::USER, "relevance"));
        append(tr("Users (name)"), SearchResource(tr("Users (name)"), Resources::USER, "alpha"));
        append(tr("Users (popular)"), SearchResource(tr("Users (popular)"), Resources::USER, "popular"));
    }
};

#endif // DAILYMOTIONSEARCHTYPEMODEL_H
