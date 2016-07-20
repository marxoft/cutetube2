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

#ifndef SERVICEMODEL_H
#define SERVICEMODEL_H

#include "selectionmodel.h"
#include "pluginmanager.h"
#include "resources.h"

class ServiceModel : public SelectionModel
{
    Q_OBJECT
    
public:
    explicit ServiceModel(QObject *parent = 0) :
        SelectionModel(parent)
    {
        reload();
        connect(PluginManager::instance(), SIGNAL(loaded(int)), this, SLOT(reload()));
    }

public Q_SLOTS:
    inline void reload() {
        clear();

        append("YouTube", Resources::YOUTUBE);
        append("Dailymotion", Resources::DAILYMOTION);
        append("Vimeo", Resources::VIMEO);

        foreach (const ServicePluginPair &pair, PluginManager::instance()->plugins()) {
            append(pair.config->displayName(), pair.config->id());
        }
    }
};

#endif // SERVICEMODEL_H
