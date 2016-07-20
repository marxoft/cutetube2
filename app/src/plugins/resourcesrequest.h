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
 
#ifndef RESOURCESREQUEST_H
#define RESOURCESREQUEST_H

#include <QObject>
#include <QVariant>

class QNetworkAccessManager;

class ResourcesRequest : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QString errorString READ errorString NOTIFY finished)
    Q_PROPERTY(QVariant result READ result NOTIFY finished)
    Q_PROPERTY(Status status READ status NOTIFY statusChanged)

    Q_ENUMS(Status)
        
public:
    enum Status {
        Null = 0,
        Loading,
        Canceled,
        Ready,
        Failed
    };
    
    explicit ResourcesRequest(QObject *parent = 0) : QObject(parent) {}

    virtual QString errorString() const = 0;

    virtual QVariant result() const = 0;
    
    virtual Status status() const = 0;

public Q_SLOTS:
    virtual bool cancel() = 0;
    virtual bool get(const QString &resourceType, const QString &resourceId) = 0;
    virtual bool list(const QString &resourceType, const QString &resourceId) = 0;
    virtual bool search(const QString &resourceType, const QString &query, const QString &order) = 0;

Q_SIGNALS:
    void statusChanged(ResourcesRequest::Status s);
    void finished();
};

#endif // RESOURCESREQUEST_H
