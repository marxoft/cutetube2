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

#include "pluginnavmodel.h"

PluginNavModel::PluginNavModel(QObject *parent) :
    QAbstractListModel(parent)
{
    m_roles[NameRole] = "name";
    m_roles[TypeRole] = "type";
    m_roles[IdRole] = "id";
#if QT_VERSION < 0x050000
    setRoleNames(m_roles);
#endif
}

QString PluginNavModel::service() const { 
    return m_service;
}
    
void PluginNavModel::setService(const QString &s) {
    if (s != service()) {
        m_service = s;
        reload();
        emit serviceChanged();
    }
}

#if QT_VERSION >= 0x050000
QHash<int, QByteArray> PluginNavModel::roleNames() const {
    return m_roles;
}
#endif

int PluginNavModel::rowCount(const QModelIndex &) const {
    return m_items.size();
}

QVariant PluginNavModel::data(const QModelIndex &index, int role) const {
    if (!index.isValid()) {
        return QVariant();
    }
    
    switch (role) {
    case NameRole:
        return m_items.at(index.row()).name;
    case TypeRole:
        return m_items.at(index.row()).type;
    case IdRole:
         return m_items.at(index.row()).id;
    default:
        return QVariant();
    }
}

QMap<int, QVariant> PluginNavModel::itemData(const QModelIndex &index) const {
    QMap<int, QVariant> map;
    map[NameRole] = data(index, NameRole);
    map[TypeRole] = data(index, TypeRole);
    map[IdRole] = data(index, IdRole);
    
    return map;
}

QVariant PluginNavModel::data(int row, const QByteArray &role) const {
    return data(index(row), m_roles.key(role));
}

QVariantMap PluginNavModel::itemData(int row) const {
    QVariantMap map;
    map["name"] = data(row, "name");
    map["type"] = data(row, "type");
    map["id"] = data(row, "id");
    
    return map;
}

int PluginNavModel::match(const QByteArray &role, const QVariant &value) const {
    for (int i = 0; i < m_items.size(); i++) {
        if (data(i, role) == value) {
            return i;
        }
    }
    
    return -1;
}

void PluginNavModel::clear() {
    if (!m_items.isEmpty()) {
        beginResetModel();
        m_items.clear();
        endResetModel();
        emit countChanged(0);
    }
}

void PluginNavModel::reload() {
    clear();
    ResourcesPlugin plugin = ResourcesPlugins::instance()->getPluginFromName(service());
    QList<ListResource> resources = plugin.listResources.values();
    
    beginResetModel();
    
    if (!plugin.searchResources.isEmpty()) {
        ListResource resource;
        resource.name = tr("Search");
        m_items << resource;
    }
    
    m_items << resources;
    endResetModel();
    emit countChanged(rowCount());
}
