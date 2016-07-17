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

#ifndef PLUGINSETTINGS_H
#define PLUGINSETTINGS_H

#include <QObject>
#include <QVariant>

class PluginSettings : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QString pluginId READ pluginId WRITE setPluginId NOTIFY changed)

public:
    explicit PluginSettings(QObject *parent = 0);
    explicit PluginSettings(const QString &pluginId, QObject *parent = 0);

    QString pluginId() const;
    void setPluginId(const QString &id);
    
public Q_SLOTS:
    bool remove(const QString &key);
    
    QVariant value(const QString &key, const QVariant &defaultValue = QVariant()) const;
    void setValue(const QString &key, const QVariant &value);

Q_SIGNALS:
    void changed();

private:
    QString m_pluginId;
};

#endif // PLUGINSETTINGS_H
