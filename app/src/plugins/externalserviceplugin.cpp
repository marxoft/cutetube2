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

#include "externalserviceplugin.h"
#include "externalresourcesrequest.h"

ExternalServicePlugin::ExternalServicePlugin(QObject *parent) :
    QObject(parent),
    ServicePlugin()
{
}

ExternalServicePlugin::ExternalServicePlugin(const QString &id, const QString &fileName, QObject *parent) :
    QObject(parent),
    ServicePlugin(),
    m_fileName(fileName),
    m_id(id)
{
}

QString ExternalServicePlugin::fileName() const {
    return m_fileName;
}

void ExternalServicePlugin::setFileName(const QString &fileName) {
    m_fileName = fileName;
}

QString ExternalServicePlugin::id() const {
    return m_id;
}

void ExternalServicePlugin::setId(const QString &id) {
    m_id = id;
}

ResourcesRequest* ExternalServicePlugin::createRequest(QObject *parent) {
    return new ExternalResourcesRequest(id(), fileName(), parent);
}
