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

#ifndef PLUGINNAVMODEL_H
#define PLUGINNAVMODEL_H

#include "selectionmodel.h"
#include "pluginmanager.h"

class PluginNavModel : public SelectionModel
{
    Q_OBJECT
    
    Q_PROPERTY(QString service READ service WRITE setService NOTIFY serviceChanged)

public:
    explicit PluginNavModel(QObject *parent = 0) :
        SelectionModel(parent)
    {
        setTextAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    }
    
    inline QString service() const { 
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
        
        if (!config->searchResources().isEmpty()) {
            append(tr("Search"), "");
        }
        
        foreach (const ListResource &resource, config->listResources()) {
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

#endif // PLUGINNAVMODEL_H
