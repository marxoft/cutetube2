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

#ifndef DBUSSERVICE_H
#define DBUSSERVICE_H

#include <QObject>
#include <QVariantMap>

class DBusService : public QObject
{
    Q_OBJECT
    
public:
    explicit DBusService(QObject *parent = 0);
    
private Q_SLOTS:
    bool showResource(const QString &url);
    
Q_SIGNALS:
    void resourceRequested(const QVariantMap &resource);
};

#endif // DBUSSERVICE_H
