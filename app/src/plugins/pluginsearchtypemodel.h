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

#ifndef PLUGINSEARCHTYPEMODEL_H
#define PLUGINSEARCHTYPEMODEL_H

#include "selectionmodel.h"
#include "resourcesplugins.h"

class PluginSearchTypeModel : public SelectionModel
{
    Q_OBJECT
    
    Q_PROPERTY(QString service READ service WRITE setService NOTIFY serviceChanged)

public:
    explicit PluginSearchTypeModel(QObject *parent = 0) :
        SelectionModel(parent)
    {
    }
    
    inline QString service() { 
        return m_service;
    
    }
    
    inline void setService(const QString &s) {
        if (s != service()) {
            m_service = s;
            reload();
            emit serviceChanged();
        }
    }
    
public Q_SLOTS:
    inline void reload() {
        clear();
        ResourcesPlugin plugin = ResourcesPlugins::instance()->getPluginFromName(service());
        
        foreach (SearchResource resource, plugin.searchResources.values()) {
            append(resource.value("name").toString(), resource);
        }
    }
    
Q_SIGNALS:
    void serviceChanged();
    
private:
    QString m_service;
};

#endif // PLUGINSEARCHTYPEMODEL_H
