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

#include "pluginmanager.h"
#include "definitions.h"
#include "externalserviceplugin.h"
#include "javascriptserviceplugin.h"
#include "logger.h"
#include <QDir>
#include <QFileInfo>
#include <QPluginLoader>

static bool displayNameLessThan(const ServicePluginPair &pair, const ServicePluginPair &other) {
    return QString::localeAwareCompare(pair.config->displayName(), other.config->displayName()) < 0;
}

PluginManager* PluginManager::self = 0;

PluginManager::PluginManager(QObject *parent) :
    QObject(parent),
    m_lastLoaded(QDateTime::fromTime_t(0))
{
}

PluginManager::~PluginManager() {
    self = 0;
}

PluginManager* PluginManager::instance() {
    return self ? self : self = new PluginManager;
}

ServicePluginList PluginManager::plugins() const {
    return m_plugins;
}

ServicePluginConfig* PluginManager::getConfigForService(const QString &service) const {
    foreach (const ServicePluginPair &pair, m_plugins) {
        if (pair.config->id() == service) {
            Logger::log("PluginManager::getConfigForService(). PluginFound: " + service, Logger::HighVerbosity);
            return pair.config;
        }
    }
    
    Logger::log("PluginManager::getConfigForService(). No Plugin found", Logger::HighVerbosity);
    return 0;
}

ServicePluginConfig* PluginManager::getConfigByFilePath(const QString &filePath) const {
    foreach (const ServicePluginPair &pair, m_plugins) {
        if (pair.config->filePath() == filePath) {
            Logger::log("PluginManager::getConfigByFilePath(). PluginFound: " + pair.config->id(),
                        Logger::HighVerbosity);
            return pair.config;
        }
    }
    
    Logger::log("PluginManager::getConfigByFilePath(). No Plugin found", Logger::HighVerbosity);
    return 0;
}

ServicePlugin* PluginManager::getPluginForService(const QString &service) const {
    foreach (const ServicePluginPair &pair, m_plugins) {
        if (pair.config->id() == service) {
            Logger::log("PluginManager::getPluginForService(). PluginFound: " + service, Logger::HighVerbosity);
            return pair.plugin;
        }
    }
    
    Logger::log("PluginManager::getPluginForService(). No Plugin found", Logger::HighVerbosity);
    return 0;
}

ResourcesRequest* PluginManager::createRequestForService(const QString &service, QObject *parent) const {
    if (ServicePlugin *plugin = getPluginForService(service)) {
        return plugin->createRequest(parent);
    }

    return 0;
}

bool PluginManager::resourceTypeIsSupported(const QString &service, const QString &resourceType,
                                            const QString &method) const {
    if (ServicePluginConfig *config = getConfigForService(service)) {
        return config->resourceTypeIsSupported(resourceType, method);
    }

    return false;
}

int PluginManager::load() {
    Logger::log("PluginManager::load(): Loading plugins modified since "
                + m_lastLoaded.toString(Qt::ISODate), Logger::LowVerbosity);
    int count = 0;
    QDir dir;
    
    foreach (const QString &path, PLUGIN_PATHS) {
        dir.setPath(path);
        
        foreach (const QFileInfo &info, dir.entryInfoList(QStringList() << "*.json", QDir::Files, QDir::Time)) {
            if (info.lastModified() > m_lastLoaded) {
                ServicePluginConfig *config = getConfigByFilePath(info.absoluteFilePath());
                
                if (!config) {
                    config = new ServicePluginConfig(this);
                    
                    if (config->load(info.absoluteFilePath())) {
                        if (config->pluginType() == "qt") {
                            QPluginLoader loader(config->pluginFilePath());
                            QObject *obj = loader.instance();
                            
                            if (obj) {
                                if (ServicePlugin *plugin = qobject_cast<ServicePlugin*>(obj)) {
                                    m_plugins << ServicePluginPair(config, plugin);
                                    ++count;
                                    Logger::log("PluginManager::load(). Qt Plugin loaded: " + config->id(),
                                                Logger::MediumVerbosity);
                                }
                                else {
                                    loader.unload();
                                    Logger::log("PluginManager::load(). Error loading Qt plugin: "
                                                + config->id());
                                }
                            }
                            else {
                                Logger::log("PluginManager::load(). Qt plugin is NULL: " + config->id());
                            }
                        }
                        else if (config->pluginType() == "js") {
                            JavaScriptServicePlugin *js =
                            new JavaScriptServicePlugin(config->id(), config->pluginFilePath(), this);
                            m_plugins << ServicePluginPair(config, js);
                            ++count;
                            Logger::log("PluginManager::load(). JavaScript plugin loaded: " + config->id(),
                                        Logger::MediumVerbosity);
                        }
                        else {
                            ExternalServicePlugin *ext =
                            new ExternalServicePlugin(config->id(), config->pluginFilePath(), this);
                            m_plugins << ServicePluginPair(config, ext);
                            ++count;
                            Logger::log("PluginManager::load(). External plugin loaded: " + config->id(),
                                        Logger::MediumVerbosity);
                        }
                    }
                    else {
                        delete config;
                    }
                }
            }
            else {
                break;
            }
        }
    }

    Logger::log(QString("PluginManager::load() %1 plugins loaded").arg(count), Logger::LowVerbosity);

    if (count > 0) {
        qSort(m_plugins.begin(), m_plugins.end(), displayNameLessThan);
        emit loaded(count);
    }

    m_lastLoaded = QDateTime::currentDateTime();
    return count;
}
