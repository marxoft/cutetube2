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

#ifndef PLUGINPLAYLISTMODEL_H
#define PLUGINPLAYLISTMODEL_H

#include <QAbstractListModel>
#include "pluginplaylist.h"

class PluginPlaylistModel : public QAbstractListModel
{
    Q_OBJECT
    
    Q_PROPERTY(bool canFetchMore READ canFetchMore NOTIFY statusChanged)
    Q_PROPERTY(int count READ rowCount NOTIFY countChanged)
    Q_PROPERTY(QString errorString READ errorString NOTIFY statusChanged)
    Q_PROPERTY(QString service READ service WRITE setService NOTIFY serviceChanged)
    Q_PROPERTY(ResourcesRequest::Status status READ status NOTIFY statusChanged)
    
public:
    enum Roles {
        DateRole = Qt::UserRole + 1,
        DescriptionRole,
        IdRole,
        LargeThumbnailUrlRole,
        ServiceRole,
        ThumbnailUrlRole,
        TitleRole,
        UserIdRole,
        UsernameRole,
        VideoCountRole
    };
    
    explicit PluginPlaylistModel(QObject *parent = 0);

    QString errorString() const;
    
    QString service() const;
    void setService(const QString &s);    
    
    ResourcesRequest::Status status() const;
    
#if QT_VERSION >= 0x050000
    QHash<int, QByteArray> roleNames() const;
#endif
    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    
    bool canFetchMore(const QModelIndex &parent = QModelIndex()) const;
    Q_INVOKABLE void fetchMore(const QModelIndex &parent = QModelIndex());
    
    QVariant data(const QModelIndex &index, int role) const;
    QMap<int, QVariant> itemData(const QModelIndex &index) const;
    
    Q_INVOKABLE QVariant data(int row, const QByteArray &role) const;
    Q_INVOKABLE QVariantMap itemData(int row) const;
    
    Q_INVOKABLE PluginPlaylist* get(int row) const;
    
    Q_INVOKABLE void list(const QString &resourceId);
    Q_INVOKABLE void search(const QString &query, const QString &order);

public Q_SLOTS:
    void clear();
    void cancel();
    void reload();

private Q_SLOTS:
    void onRequestFinished();
    
Q_SIGNALS:
    void countChanged(int count);
    void serviceChanged();
    void statusChanged(ResourcesRequest::Status status);
    
private:
    void append(PluginPlaylist *playlist);
    void insert(int row, PluginPlaylist *playlist);
    void remove(int row);

    ResourcesRequest* request();
    
    ResourcesRequest *m_request;
    
    QString m_resourceId;
    QString m_service;
    QString m_query;
    QString m_order;
    QString m_next;
        
    QList<PluginPlaylist*> m_items;
    
    QHash<int, QByteArray> m_roles;
};
    
#endif // PLUGINPLAYLISTMODEL_H
