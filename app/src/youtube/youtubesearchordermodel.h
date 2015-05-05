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

#ifndef YOUTUBESEARCHORDERMODEL_H
#define YOUTUBESEARCHORDERMODEL_H

#include "selectionmodel.h"

class YouTubeSearchOrderModel : public SelectionModel
{
    Q_OBJECT

public:
    explicit YouTubeSearchOrderModel(QObject *parent = 0) :
        SelectionModel(parent)
    {
        append(tr("Relevance"), "relevance");
        append(tr("Date"), "date");
        append(tr("Popular"), "viewCount");
        append(tr("Rating"), "rating");
        append(tr("Title"), "title");
    }
};

#endif // YOUTUBESEARCHORDERMODEL_H
