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

#ifndef PLUGINCATEGORYMODEL_H
#define PLUGINCATEGORYMODEL_H

#include "selectionmodel.h"
#include "resourcesrequest.h"

class PluginCategoryModel : public SelectionModel
{
    Q_OBJECT
    
    Q_PROPERTY(bool canFetchMore READ canFetchMore NOTIFY statusChanged)
    Q_PROPERTY(QString errorString READ errorString NOTIFY statusChanged)
    Q_PROPERTY(QString service READ service WRITE setService NOTIFY serviceChanged)
    Q_PROPERTY(ResourcesRequest::Status status READ status NOTIFY statusChanged)
    
public:
    explicit PluginCategoryModel(QObject *parent = 0);
    
    bool canFetchMore(const QModelIndex &parent = QModelIndex()) const;
    Q_INVOKABLE void fetchMore(const QModelIndex &parent = QModelIndex());

    QString errorString() const;
    
    QString service() const;
    void setService(const QString &s);    
    
    ResourcesRequest::Status status() const;
    
    Q_INVOKABLE void list(const QString &resourceId);
    Q_INVOKABLE void search(const QString &query, const QString &order);
        
public Q_SLOTS:
    void cancel();
    void reload();

private Q_SLOTS:
    void onRequestFinished();
    
Q_SIGNALS:
    void serviceChanged();
    void statusChanged(ResourcesRequest::Status s);
        
private:
    ResourcesRequest* request();
    
    ResourcesRequest *m_request;

    QString m_service;
    QString m_resourceId;
    QString m_query;
    QString m_order;
    QString m_next;
};
    
#endif // PLUGINCATEGORYMODEL_H
