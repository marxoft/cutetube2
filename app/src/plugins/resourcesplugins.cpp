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

bool ResourcesPlugins::resourceTypeIsSupported(const QString &pluginName, const QString &resource,
                                               const QString &method) const {
    if (method == "list") {
        return getPluginFromName(pluginName).listResources.contains(resource);
    }

    if (method == "search") {
        return getPluginFromName(pluginName).searchResources.contains(resource);
    }
    
    if (method == "get") {
        return getPluginFromName(pluginName).regExps.contains(resource);
    }
    
    return false;
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
                        ListResource listResource(resourceElem.attribute("name"), resourceElem.attribute("type"),
                                                  resourceElem.attribute("id"));
                        plugin.listResources.insert(resourceElem.attribute("type"), listResource);
                    }
                    else if (method == "search") {
                        SearchResource searchResource(resourceElem.attribute("name"), resourceElem.attribute("type"),
                                                      resourceElem.attribute("order"));
                        plugin.searchResources.insert(resourceElem.attribute("type"), searchResource);
                    }
                    else if (method == "get") {
                        plugin.regExps[resourceElem.attribute("type")] = QRegExp(resourceElem.attribute("regexp"));
                    }
                }
                
                m_plugins[name] = plugin;
            }
        }
    }
}
