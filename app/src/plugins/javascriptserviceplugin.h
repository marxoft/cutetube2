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

#ifndef JAVASCRIPTSERVICEPLUGIN_H
#define JAVASCRIPTSERVICEPLUGIN_H

#include "serviceplugin.h"

class JavaScriptServicePlugin : public QObject, public ServicePlugin
{
    Q_OBJECT

    Q_PROPERTY(QString fileName READ fileName WRITE setFileName)
    Q_PROPERTY(QString id READ id WRITE setId)
    
    Q_INTERFACES(ServicePlugin)

public:
    explicit JavaScriptServicePlugin(QObject *parent = 0);
    explicit JavaScriptServicePlugin(const QString &id, const QString &fileName, QObject *parent = 0);
    
    QString fileName() const;
    void setFileName(const QString &fileName);

    QString id() const;
    void setId(const QString &id);
    
    virtual ResourcesRequest* createRequest(QObject *parent = 0);

private:
    QString m_fileName;
    QString m_id;
};

#endif // JAVASCRIPTSERVICEPLUGIN_H
