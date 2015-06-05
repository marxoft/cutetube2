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
#include "resourcesplugins.h"

class PluginSearchOrderModel : public SelectionModel
{
    Q_OBJECT
    
    Q_PROPERTY(QString resourceType READ resourceType WRITE setResourceType NOTIFY resourceTypeChanged)
    Q_PROPERTY(QString service READ service WRITE setService NOTIFY serviceChanged)

public:
    explicit PluginSearchOrderModel(QObject *parent = 0) :
        SelectionModel(parent)
    {
    }
    
    inline QString resourceType() const {
        return m_resource;
    }
    
    inline void setResourceType(const QString &r) {
        if (r != resourceType()) {
            m_resource = r;
            emit resourceTypeChanged();
            
            if (!service().isEmpty()) {
                reload();
            }
        }
    }
    
    inline QString service() const { 
        return m_service;
    }
    
    inline void setService(const QString &s) {
        if (s != service()) {
            m_service = s;
            emit serviceChanged();
            
            if (!resourceType().isEmpty()) {
                reload();
            }
        }
    }
    
public Q_SLOTS:
    inline void reload() {
        clear();
        ResourcesPlugin plugin = ResourcesPlugins::instance()->getPluginFromName(service());
        QList< QPair<QString, QString> > sortOrders = plugin.sortOrders.value(resourceType());
        
        for (int i = 0; i < sortOrders.size(); i++) {
            append(sortOrders.at(i).first, sortOrders.at(i).second);
        }
    }
    
Q_SIGNALS:
    void resourceTypeChanged();
    void serviceChanged();
    
private:
    QString m_resource;
    QString m_service;
};

#endif // PLUGINSEARCHORDERMODEL_H
