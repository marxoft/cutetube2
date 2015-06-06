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

#ifndef PLUGINNAVMODEL_H
#define PLUGINNAVMODEL_H

#include "resourcesplugins.h"
#include <QAbstractListModel>

class PluginNavModel : public QAbstractListModel
{
    Q_OBJECT
    
    Q_PROPERTY(int count READ rowCount NOTIFY countChanged)
    Q_PROPERTY(QString service READ service WRITE setService NOTIFY serviceChanged)
    
public:
    enum Roles {
        NameRole = Qt::DisplayRole,
        TypeRole = Qt::UserRole,
        IdRole = Qt::UserRole + 1
    };
    
    explicit PluginNavModel(QObject *parent = 0);
    
    QString service() const;
    void setService(const QString &s);
        
#if QT_VERSION >= 0x050000
    QHash<int, QByteArray> roleNames() const;
#endif
    
    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    
    QVariant data(const QModelIndex &index, int role = NameRole) const;
    QMap<int, QVariant> itemData(const QModelIndex &index) const;
    
    Q_INVOKABLE QVariant data(int row, const QByteArray &role) const;
    Q_INVOKABLE QVariantMap itemData(int row) const;
    
    Q_INVOKABLE int match(const QByteArray &role, const QVariant &value) const;

public Q_SLOTS:
    void clear();
    void reload();
    
Q_SIGNALS:
    void countChanged(int c);
    void serviceChanged();
    
protected:
    QList<ListResource> m_items;
    
    QHash<int, QByteArray> m_roles;
    
    QString m_service;
};

#endif // PLUGINNAVMODEL_H
