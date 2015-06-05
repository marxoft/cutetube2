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

#ifndef RESOURCESPLUGINS_H
#define RESOURCESPLUGINS_H

#include <QObject>
#include <QMap>
#include <QStringList>
#include <QRegExp>

struct ResourcesPlugin {
    QString name;
    QString command;
    QString settings;
    QMap<QString, QString> listResources;
    QMap<QString, QString> searchResources;
    QMap<QString, QList< QPair<QString, QString> > > sortOrders;
    QMap<QString, QRegExp> regExps;
};

class ResourcesPlugins : public QObject
{
    Q_OBJECT
    
public:
    explicit ResourcesPlugins(QObject *parent = 0);
    ~ResourcesPlugins();
    
    static ResourcesPlugins* instance();
    
    ResourcesPlugin getPluginFromName(const QString &name) const;
    
    QList<ResourcesPlugin> plugins() const;
    
    QStringList pluginNames() const;
    
    Q_INVOKABLE bool resourceTypeIsSupported(const QString &name, const QString &resourceType,
                                             const QString &method = QString("list")) const;
    
public Q_SLOTS:
    void load();
    
private:
    static ResourcesPlugins *self;
    
    QMap<QString, ResourcesPlugin> m_plugins;
};

#endif // RESOURCESPLUGINS_H
