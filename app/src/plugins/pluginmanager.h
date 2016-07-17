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

#ifndef PLUGINMANAGER_H
#define PLUGINMANAGER_H

#include "serviceplugin.h"
#include "servicepluginconfig.h"
#include <QDateTime>

struct ServicePluginPair
{
    ServicePluginPair(ServicePluginConfig *c, ServicePlugin* p) :
        config(c),
        plugin(p)
    {
    }

    ServicePluginConfig *config;
    ServicePlugin *plugin;
};

typedef QList<ServicePluginPair> ServicePluginList;

class PluginManager : public QObject
{
    Q_OBJECT

public:
    explicit PluginManager(QObject *parent = 0);
    ~PluginManager();

    static PluginManager* instance();

    ServicePluginList plugins() const;

    ServicePluginConfig* getConfigForService(const QString &service) const;

    ServicePlugin* getPluginForService(const QString &service) const;

public Q_SLOTS:
    ResourcesRequest* createRequestForService(const QString &service, QObject *parent = 0) const;

    bool resourceTypeIsSupported(const QString &service, const QString &resourceType,
                                 const QString &method = QString("list")) const;

    int load();

private:
    ServicePluginConfig* getConfigByFilePath(const QString &filePath) const;

    static PluginManager *self;

    QDateTime m_lastLoaded;

    ServicePluginList m_plugins;
};

#endif // PLUGINMANAGER_H
