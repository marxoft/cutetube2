/*
 * Copyright (C) 2016 Stuart Howarth <showarth@marxoft.co.uk>
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms and conditions of the GNU General Public License,
 * version 3, as published by the Free Software Foundation.
 *
 * This program is distributed in the hope it will be useful, but WITHOUT ANY
 * WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software Foundation,
 * Inc., 51 Franklin St - Fifth Floor, Boston, MA 02110-1301 USA.
 */

#ifndef PLUGINSEARCHTYPEMODEL_H
#define PLUGINSEARCHTYPEMODEL_H

#include "selectionmodel.h"
#include "pluginmanager.h"

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
        const ServicePluginConfig *config = PluginManager::instance()->getConfigForService(service());

        if (!config) {
            return;
        }
        
        foreach (const SearchResource &resource, config->searchResources()) {
            const QString label = resource.label();
            
            if (!label.isEmpty()) {
                append(label, resource);
            }
        }
    }
    
Q_SIGNALS:
    void serviceChanged();
    
private:
    QString m_service;
};

#endif // PLUGINSEARCHTYPEMODEL_H
