/*
 * Copyright (C) 2014 Stuart Howarth <showarth@marxoft.co.uk>
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

#include "categorymodel.h"

CategoryModel::CategoryModel(QObject *parent) :
    QAbstractTableModel(parent)
{
#if QT_VERSION >= 0x040600
    m_roleNames[NameRole] = "name";
    m_roleNames[PathRole] = "path";
#if QT_VERSION < 0x050000
    setRoleNames(m_roleNames);
#endif
#endif
    reload();
    connect(Settings::instance(), SIGNAL(categoriesChanged()), this, SLOT(reload()));
}

#if QT_VERSION >= 0x050000
QHash<int, QByteArray> CategoryModel::roleNames() const {
    return m_roleNames;
}
#endif

int CategoryModel::rowCount(const QModelIndex &) const {
    return m_list.size();
}

int CategoryModel::columnCount(const QModelIndex &) const {
    return 2;
}

QVariant CategoryModel::headerData(int section, Qt::Orientation orientation, int role) const {
    if ((role != Qt::DisplayRole) || (orientation != Qt::Horizontal)) {
        return QVariant();
    }

    switch (section) {
    case 0:
        return tr("Name");
    case 1:
        return tr("Download path");
    default:
        return QVariant();
    }
}

QVariant CategoryModel::data(const QModelIndex &index, int role) const {
    switch (role) {
    case NameRole:
        return m_list.at(index.row()).name;
    case PathRole:
        return m_list.at(index.row()).path;
    case Qt::DisplayRole:
        switch (index.column()) {
        case 0:
            return m_list.at(index.row()).name;
        case 1:
            return m_list.at(index.row()).path;
        default:
            return QVariant();
        }
    default:
        return QVariant();
    }
}

QMap<int, QVariant> CategoryModel::itemData(const QModelIndex &index) const {
    QMap<int, QVariant> map;
    map[NameRole] = data(index, NameRole);
    map[PathRole] = data(index, PathRole);
    
    return map;
}

QVariant CategoryModel::data(int row, const QByteArray &role) const {
    return data(index(row, 0), roleNames().key(role));
}

QVariantMap CategoryModel::itemData(int row) const {
    QVariantMap map;
    map["name"] = data(row, "name");
    map["path"] = data(row, "path");
    
    return map;
}

void CategoryModel::addCategory(const QString &name, const QString &path) {
    Settings::addCategory(name, path);
}

void CategoryModel::removeCategory(const QString &name) {
    Settings::removeCategory(name);
}

void CategoryModel::removeCategory(int row) {
    removeCategory(data(index(row, 0), NameRole).toString());
}

void CategoryModel::clear() {
    if (!m_list.isEmpty()) {
        beginResetModel();
        m_list.clear();
        endResetModel();
    }
}

void CategoryModel::reload() {
    clear();
    beginResetModel();
    m_list = Settings::categories();
    endResetModel();
    
    emit countChanged(rowCount());
}
