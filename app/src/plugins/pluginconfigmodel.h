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

#ifndef PLUGINCONFIGMODEL_H
#define PLUGINCONFIGMODEL_H

#include "pluginmanager.h"
#include <QAbstractListModel>

class PluginConfigModel : public QAbstractListModel
{
    Q_OBJECT
    
    Q_PROPERTY(int count READ rowCount NOTIFY countChanged)
    
public:
    enum Roles {
        DisplayNameRole = Qt::DisplayRole,
        FilePathRole = Qt::UserRole + 1,
        IdRole,
        PluginFilePathRole,
        PluginTypeRole,
        SettingsRole,
        VersionRole
    };
    
    explicit PluginConfigModel(QObject *parent = 0);
        
#if QT_VERSION >= 0x050000
    QHash<int, QByteArray> roleNames() const;
#endif
    
    int rowCount(const QModelIndex &parent = QModelIndex()) const;
        
    QVariant data(const QModelIndex &index, int role = DisplayNameRole) const;
    QMap<int, QVariant> itemData(const QModelIndex &index) const;
    
    Q_INVOKABLE virtual QVariant data(int row, const QByteArray &role) const;
    Q_INVOKABLE virtual QVariantMap itemData(int row) const;
    
    QModelIndexList match(const QModelIndex &start, int role, const QVariant &value, int hits = 1,
                          Qt::MatchFlags flags = Qt::MatchFlags(Qt::MatchExactly | Qt::MatchWrap)) const;
    Q_INVOKABLE virtual int match(int start, const QByteArray &role, const QVariant &value,
                                  int flags = Qt::MatchFlags(Qt::MatchExactly | Qt::MatchWrap)) const;
    
public Q_SLOTS:
    void clear();
    void reload();
    
Q_SIGNALS:
    void countChanged(int count);
    
protected:
    ServicePluginList m_items;
    QHash<int, QByteArray> m_roles;
};

#endif // PLUGINCONFIGMODEL_H
