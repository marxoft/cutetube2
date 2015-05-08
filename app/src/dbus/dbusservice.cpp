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

#include "dbusservice.h"
#include "resources.h"
#include <QDBusConnection>

DBusService::DBusService(QObject *parent) :
    QObject(parent)
{
    QDBusConnection connection = QDBusConnection::sessionBus();
    connection.registerService("org.marxoft.cutetube2");
    connection.registerObject("/", this, QDBusConnection::ExportAllSlots);
}

bool DBusService::showResource(const QString &url) {
    QVariantMap resource = Resources::getResourceFromUrl(url);
    
    if (resource.isEmpty()) {
        return false;
    }
    
    emit resourceRequested(resource);
    return true;
}
