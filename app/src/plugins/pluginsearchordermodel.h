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

#ifndef PLUGINSEARCHORDERMODEL_H
#define PLUGINSEARCHORDERMODEL_H

#include "selectionmodel.h"
#include "resources.h"

class PluginSearchOrderModel : public SelectionModel
{
    Q_OBJECT

public:
    explicit PluginSearchOrderModel(QObject *parent = 0) :
        SelectionModel(parent)
    {
        append(tr("Relevance"), Resources::RELEVANCE);
        append(tr("Date"), Resources::DATE);
        append(tr("Popular"), Resources::POPULAR);
        append(tr("Alphabetical"), Resources::ALPHABET);
    }
};

#endif // SEARCHORDERMODEL_H
