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
#include <QDomDocument>
#include <QDomElement>
#include <QFile>
#include <QDir>
#ifdef CUTETUBE_DEBUG
#include <QDebug>
#endif

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

QList<ResourcesPlugin> ResourcesPlugins::plugins() const {
    return m_plugins.values();
}

QStringList ResourcesPlugins::pluginNames() const {
    return m_plugins.keys();
}

bool ResourcesPlugins::resourceTypeIsSupported(const QString &name, const QString &resource,
                                               const QString &method) const {
    if (method == "search") {
        return getPluginFromName(name).searchResources.contains(resource);
    }
    
    if (method == "get") {
        return getPluginFromName(name).regExps.contains(resource);
    }
    
    return getPluginFromName(name).listResources.contains(resource);
}

void ResourcesPlugins::load() {
    m_plugins.clear();
    QDir dir;

    foreach (QString path, PLUGIN_PATHS) {
        dir.setPath(path);

        foreach (QString fileName, dir.entryList(QStringList() << "*.plugin", QDir::Files)) {
#ifdef CUTETUBE_DEBUG
            qDebug() << "ResourcesPlugins::load: Plugin found:" << fileName;
#endif
            QDomDocument doc;
            QFile file(dir.absoluteFilePath(fileName));
            
            if (!file.open(QIODevice::ReadOnly)) {
#ifdef CUTETUBE_DEBUG
                qDebug() << "ResourcesPlugins::load: File error:" << file.errorString();
#endif
                continue;
            }
            
            if (!doc.setContent(&file)) {
                file.close();
#ifdef CUTETUBE_DEBUG
                qDebug() << "ResourcesPlugins::load: XML error";
#endif
                continue;
            }
        
            
            QDomElement docElem = doc.documentElement();
            QString name = docElem.attribute("name");
            QString command = docElem.attribute("exec");
            QDomNodeList resources = docElem.elementsByTagName("resource");
            
            if ((!name.isEmpty()) && (!command.isEmpty()) && (!resources.isEmpty())) {
                ResourcesPlugin plugin;
                plugin.name = name;
                plugin.command = command;
                
                if (docElem.hasAttribute("settings")) {
                    QString settings = docElem.attribute("settings");
                    plugin.settings = settings.startsWith('/') ? settings : path + settings;
                }
                
                for (int i = 0; i < resources.size(); i++) {
                    QDomElement resourceElem = resources.at(i).toElement();
                    QString method = resourceElem.attribute("method");
                    
                    if (method == "list") {
                        plugin.listResources[resourceElem.attribute("type")] = resourceElem.attribute("name");
                    }
                    if (method == "search") {
                        plugin.searchResources[resourceElem.attribute("type")] = resourceElem.attribute("name");
                    }
                    else if (method == "get") {
                        plugin.regExps[resourceElem.attribute("type")] = QRegExp(resourceElem.attribute("regexp"));
                    }
                }
                
                QDomNodeList sorts = docElem.elementsByTagName("sort");
                
                for (int i = 0; i < sorts.size(); i++) {
                    QDomElement sortElem = sorts.at(i).toElement();
                    plugin.sortOrders[sortElem.attribute("type")] << QPair<QString, QString>(sortElem.attribute("name"),
                                                                                             sortElem.attribute("value"));
                }
#ifdef CUTETUBE_DEBUG
                qDebug() << "ResourcesPlugins::load: Plugin loaded:" << plugin.name << plugin.settings << plugin.command
                                                                     << plugin.listResources << plugin.searchResources
                                                                     << plugin.regExps << plugin.sortOrders;
#endif
                
                m_plugins[name] = plugin;
            }
        }
    }
}
