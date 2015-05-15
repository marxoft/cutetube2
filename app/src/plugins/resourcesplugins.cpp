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

#include "resourcesplugins.h"
#include "definitions.h"
#include <QDir>
#include <QSettings>

ResourcesPlugins* ResourcesPlugins::self = 0;

ResourcesPlugins::ResourcesPlugins(QObject *parent) :
    QObject(parent)
{
    if (!self) {
        self = this;
    }
}

ResourcesPlugins::~ResourcesPlugins() {
    if (self == this) {
        self = 0;
    }
}

ResourcesPlugins* ResourcesPlugins::instance() {
    return self;
}

ResourcesPlugin ResourcesPlugins::getPluginFromName(const QString &name) const {
    return m_plugins.value(name);
}

ResourcesPlugin ResourcesPlugins::getPluginFromUrl(const QString &url) const {
    QMapIterator<QString, ResourcesPlugin> iterator(m_plugins);
    
    ResourcesPlugin plugin;
    
    while (iterator.hasNext()) {
        iterator.next();
        QRegExp re = iterator.value().urlRegExp;
        
        if ((!re.isEmpty()) && (re.indexIn(url) == 0)) {
            plugin = iterator.value();
            break;
        }
    }
    
    return plugin;
}

QList<ResourcesPlugin> ResourcesPlugins::plugins() const {
    return m_plugins.values();
}

QStringList ResourcesPlugins::pluginNames() const {
    return m_plugins.keys();
}

bool ResourcesPlugins::resourceTypeIsSupported(const QString &name, const QString &resource,
                                               const QString &method) const {
    if (method == "search") {
        return getPluginFromName(name).supportedSearchResources.contains(resource);
    }
    
    return getPluginFromName(name).supportedListResources.contains(resource);
}

void ResourcesPlugins::load() {
    m_plugins.clear();
    
    QDir dir(PLUGIN_PATH);
    
    foreach (QString fileName, dir.entryList(QStringList() << "*.plugin", QDir::Files, QDir::Name | QDir::IgnoreCase)) {
        QSettings loader(dir.absoluteFilePath(fileName), QSettings::IniFormat);
        QString name = loader.value("Name").toString();
        QString command = loader.value("Exec").toString();
        QStringList supportedListResources = loader.value("SupportedListResources").toStringList();
        QStringList supportedSearchResources = loader.value("SupportedSearchResources").toStringList();
        
        if ((!name.isEmpty()) && (!command.isEmpty())
            && ((!supportedListResources.isEmpty()) || (!supportedSearchResources.isEmpty()))) {
            
            ResourcesPlugin plugin;
            plugin.name = name;
            plugin.command = command;
            plugin.supportedListResources = supportedListResources;
            plugin.supportedSearchResources = supportedSearchResources;
            
            if (loader.contains("UrlRegExp")) {
                plugin.urlRegExp = loader.value("UrlRegExp").toRegExp();
            }
                        
            if (loader.contains("Settings")) {
                 QString settings = loader.value("Settings").toString();
                 plugin.settings = settings.startsWith('/') ? settings : PLUGIN_PATH + settings;
            }
            
            m_plugins[name] = plugin;
        }
    }
}
