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

#ifndef VIMEOSEARCHTYPEMODEL_H
#define VIMEOSEARCHTYPEMODEL_H

#include "selectionmodel.h"
#include "resources.h"

class VimeoSearchTypeModel : public SelectionModel
{
    Q_OBJECT

public:
    explicit VimeoSearchTypeModel(QObject *parent = 0) :
        SelectionModel(parent)
    {
        append(tr("Videos (date)"), SearchResource(tr("Videos (date)"), Resources::VIDEO, "date"));
        append(tr("Videos (relevance)"), SearchResource(tr("Videos (relevance)"), Resources::VIDEO, "relevant"));
        append(tr("Videos (duration)"), SearchResource(tr("Videos (duration)"), Resources::VIDEO, "duration"));
        append(tr("Videos (likes)"), SearchResource(tr("Videos (likes)"), Resources::VIDEO, "likes"));
        append(tr("Videos (title)"), SearchResource(tr("Videos (title)"), Resources::VIDEO, "alphabetical"));
        append(tr("Videos (views)"), SearchResource(tr("Videos (views)"), Resources::VIDEO, "plays"));
        append(tr("Users (date)"), SearchResource(tr("Users (date)"), Resources::USER, "date"));
        append(tr("Users (relevance)"), SearchResource(tr("Users (relevance)"), Resources::USER, "relevant"));
        append(tr("Users (name)"), SearchResource(tr("Users (name)"), Resources::USER, "alphabetical"));
    }
};

#endif // VIMEOSEARCHTYPEMODEL_H
