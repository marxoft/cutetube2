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

#ifndef DBUSSERVICE_H
#define DBUSSERVICE_H

#include <QObject>
#include <QVariantMap>

class DBusService : public QObject
{
    Q_OBJECT
    
    Q_PROPERTY(QVariantMap requestedResource READ requestedResource NOTIFY resourceRequested)
    
    Q_CLASSINFO("D-Bus Interface", "org.marxoft.cutetube2")
    
public:
    explicit DBusService(QObject *parent = 0);
    
    QVariantMap requestedResource() const;
    
public Q_SLOTS:
    Q_SCRIPTABLE bool showResource(const QString &url);
    Q_SCRIPTABLE bool showResource(const QStringList &url);
    
Q_SIGNALS:
    void resourceRequested(const QVariantMap &resource);
    
private:
    QVariantMap m_resource;
};

#endif // DBUSSERVICE_H
