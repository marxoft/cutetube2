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

#ifndef VBOX7PLUGIN_H
#define VBOX7PLUGIN_H

#include "serviceplugin.h"
#include "vbox7request.h"
#if QT_VERSION < 0x050000
#include <QtPlugin>
#endif

class Vbox7Plugin : public QObject, public ServicePlugin
{
    Q_OBJECT
    Q_INTERFACES(ServicePlugin)
#if QT_VERSION >= 0x050000
    Q_PLUGIN_METADATA(IID "org.cutetube2.Vbox7Plugin")
#endif

public:
    virtual ResourcesRequest* createRequest(QObject *parent = 0) { return new Vbox7Request(parent); }
};

#if QT_VERSION < 0x050000
Q_EXPORT_PLUGIN2(cutetube2-vbox7, Vbox7Plugin)
#endif

#endif // VBOX7PLUGIN_H
