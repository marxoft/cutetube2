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

#ifndef SERVICEPLUGINCONFIG_H
#define SERVICEPLUGINCONFIG_H

#include "resources.h"
#include <QVariantList>

class ServicePluginConfig : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QString displayName READ displayName NOTIFY changed)
    Q_PROPERTY(QString filePath READ filePath NOTIFY changed)
    Q_PROPERTY(QString id READ id NOTIFY changed)
    Q_PROPERTY(QString pluginFilePath READ pluginFilePath NOTIFY changed)
    Q_PROPERTY(QString pluginType READ pluginType NOTIFY changed)
    Q_PROPERTY(QList<GetResource> getResources READ getResources NOTIFY changed)
    Q_PROPERTY(QList<ListResource> listResources READ listResources NOTIFY changed)
    Q_PROPERTY(QList<SearchResource> searchResources READ searchResources NOTIFY changed)
    Q_PROPERTY(QVariantList settings READ settings NOTIFY changed)
    Q_PROPERTY(int version READ version NOTIFY changed)

public:
    explicit ServicePluginConfig(QObject *parent = 0);

    QString displayName() const;
    
    QString filePath() const;
    
    QString id() const;

    QString pluginFilePath() const;
    
    QString pluginType() const;

    QList<GetResource> getResources() const;
    QList<ListResource> listResources() const;
    QList<SearchResource> searchResources() const;    
    
    QVariantList settings() const;
    
    int version() const;

public Q_SLOTS:
    bool resourceTypeIsSupported(const QString &resourceType, const QString &method = QString("list")) const;
    
    bool load(const QString &filePath);

Q_SIGNALS:
    void changed();

private:
    QString m_displayName;
    QString m_filePath;
    QString m_id;
    QString m_pluginFilePath;
    QString m_pluginType;
    
    QList<GetResource> m_getResources;
    QList<ListResource> m_listResources;
    QList<SearchResource> m_searchResources;
    QVariantList m_settings;
    
    int m_version;
};

#endif // SERVICEPLUGINCONFIG_H
