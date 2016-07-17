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

#include "servicepluginconfig.h"
#include "definitions.h"
#include "json.h"
#include "logger.h"
#include <QFile>

ServicePluginConfig::ServicePluginConfig(QObject *parent) :
    QObject(parent),
    m_version(1)
{
}

QString ServicePluginConfig::displayName() const {
    return m_displayName;
}

QString ServicePluginConfig::filePath() const {
    return m_filePath;
}

QString ServicePluginConfig::id() const {
    return m_id;
}

QString ServicePluginConfig::pluginFilePath() const {
    return m_pluginFilePath;
}

QString ServicePluginConfig::pluginType() const {
    return m_pluginType;
}

QVariantList ServicePluginConfig::settings() const {
    return m_settings;
}

int ServicePluginConfig::version() const {
    return m_version;
}

bool ServicePluginConfig::load(const QString &filePath) {
    m_filePath = filePath;
    QFile file(filePath);

    if ((!file.exists()) || (!file.open(QFile::ReadOnly))) {
        Logger::log("ServicePluginConfig::load(): Unable to open config file: " + filePath);
        return false;
    }

    bool ok;
    const QVariant v = QtJson::Json::parse(QString::fromUtf8(file.readAll()), ok);
    file.close();

    if (!ok) {
        Logger::log("ServicePluginConfig::load(): Error parsing config file: " + filePath);
        return false;
    }

    const QVariantMap config = v.toMap();

    if (!config.contains("name")) {
        Logger::log("ServicePluginConfig::load(): 'name' parameter is missing");
        return false;
    }
    
    if (!config.contains("resources")) {
        Logger::log("ServicePluginConfig::load(): 'resources' parameter is missing");
        return false;
    }

    Logger::log("ServicePluginConfig::load(): Config file loaded: " + filePath);    
    const int slash = filePath.lastIndexOf("/");
    const QString fileName = filePath.mid(slash + 1);
    const int dot = fileName.lastIndexOf(".");
    m_displayName = config.value("name").toString();
    m_id = fileName.left(dot);
    m_pluginType = config.value("type").toString();
    m_settings = config.value("settings").toList();
    m_version = qMax(1, config.value("version").toInt());
    
    if (m_pluginType == "js") {
        m_pluginFilePath = filePath.left(slash + 1) + m_id + ".js";
    }
    else {
        m_pluginFilePath = filePath.left(slash + 1) + LIB_PREFIX + m_id + LIB_SUFFIX;
    }

    m_getResources.clear();
    m_listResources.clear();
    m_searchResources.clear();

    foreach (const QVariant &v, config.value("resources").toList()) {
        const QVariantMap resource = v.toMap();
        const QString method = resource.value("method").toString();

        if (method == "get") {
            m_getResources << GetResource(resource);
        }
        else if (method == "list") {
            m_listResources << ListResource(resource);
        }
        else if (method == "search") {
            m_searchResources << SearchResource(resource);
        }
    }

    emit changed();
    return true;
}

QList<GetResource> ServicePluginConfig::getResources() const {
    return m_getResources;
}

QList<ListResource> ServicePluginConfig::listResources() const {
    return m_listResources;
}

QList<SearchResource> ServicePluginConfig::searchResources() const {
    return m_searchResources;
}

bool ServicePluginConfig::resourceTypeIsSupported(const QString &resourceType, const QString &method) const {
    if (method == "get") {
        foreach (const GetResource &resource, m_getResources) {
            if (resource.type() == resourceType) {
                return true;
            }
        }
    }
    else if (method == "list") {
        foreach (const ListResource &resource, m_listResources) {
            if (resource.type() == resourceType) {
                return true;
            }
        }
    }
    else if (method == "search") {
        foreach (const SearchResource &resource, m_searchResources) {
            if (resource.type() == resourceType) {
                return true;
            }
        }
    }

    return false;
}
