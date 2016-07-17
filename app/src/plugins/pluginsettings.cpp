/*
 * Copyright (C) 2016 Stuart Howarth <showarth@marxoft.co.uk>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 3 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "pluginsettings.h"
#include "definitions.h"
#include <QSettings>

PluginSettings::PluginSettings(QObject *parent) :
    QObject(parent)
{
}

PluginSettings::PluginSettings(const QString &pluginId, QObject *parent) :
    QObject(parent),
    m_pluginId(pluginId)
{
}

QString PluginSettings::pluginId() const {
    return m_pluginId;
}

void PluginSettings::setPluginId(const QString &id) {
    if (id != pluginId()) {
        m_pluginId = id;
        emit changed();
    }
}

bool PluginSettings::remove(const QString &key) {
    if ((!pluginId().isEmpty()) && (key != value(key))) {
        QSettings settings(PLUGIN_CONFIG_PATH + pluginId(), QSettings::IniFormat);

        if (settings.contains(key)) {
            settings.remove(key);
            emit changed();
            return true;
        }
    }

    return false;
}

QVariant PluginSettings::value(const QString &key, const QVariant &defaultValue) const {
    if (pluginId().isEmpty()) {
        return QVariant();
    }
    
    const QVariant v = QSettings(PLUGIN_CONFIG_PATH + pluginId(), QSettings::IniFormat).value(key, defaultValue);
    
    if (v.type() == QVariant::String) {
        if ((v == "true") || (v == "false")) {
            /* It seems that the type of a boolean value is QVariant::String when read from the file, which causes 
               problems in the scripting environment, so we convert it to a boolean.
             */
            return v.toBool();
        }
    }
    
    return v;
}

void PluginSettings::setValue(const QString &key, const QVariant &value) {
    if ((!pluginId().isEmpty()) && (key != this->value(key))) {
        QSettings(PLUGIN_CONFIG_PATH + pluginId(), QSettings::IniFormat).setValue(key, value);
        emit changed();
    }
}
