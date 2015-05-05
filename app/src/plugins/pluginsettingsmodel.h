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

#ifndef PLUGINSETTINGSMODEL_H
#define PLUGINSETTINGSMODEL_H

#include "selectionmodel.h"
#include "resourcesplugins.h"

class PluginSettingsModel : public SelectionModel
{
    Q_OBJECT
    
public:
    explicit PluginSettingsModel(QObject *parent = 0) :
        SelectionModel(parent)
    {
        reload();
    }
    
public Q_SLOTS:
    inline void reload() {
        clear();
        QList<ResourcesPlugin> plugins = ResourcesPlugins::instance()->plugins();
        
        for (int i = 0; i < plugins.size(); i++) {
            if (!plugins.at(i).settings.isEmpty()) {
                append(plugins.at(i).name, plugins.at(i).settings);
            }
        }
    }
};

#endif // PLUGINSETTINGSMODEL_H
