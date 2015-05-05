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

#include "selectionmodel.h"

SelectionModel::SelectionModel(QObject *parent) :
    QAbstractListModel(parent),
    m_alignment(Qt::AlignCenter)
{
    m_roles[NameRole] = "name";
    m_roles[ValueRole] = "value";
#if QT_VERSION < 0x050000
    setRoleNames(m_roles);
#endif
}

#if QT_VERSION >= 0x050000
QHash<int, QByteArray> SelectionModel::roleNames() const {
    return m_roles;
}
#endif

int SelectionModel::rowCount(const QModelIndex &) const {
    return m_items.size();
}

Qt::Alignment SelectionModel::textAlignment() const {
    return m_alignment;
}

void SelectionModel::setTextAlignment(Qt::Alignment align) {
    if (align != textAlignment()) {
        m_alignment = align;
        emit textAlignmentChanged();
        
        if (!m_items.isEmpty()) {
            emit dataChanged(index(0), index(m_items.size() - 1));
        }
    }
}

QVariant SelectionModel::data(const QModelIndex &index, int role) const {
    if (!index.isValid()) {
        return QVariant();
    }
    
    switch (role) {
    case Qt::TextAlignmentRole:
        return QVariant(textAlignment());
    case NameRole:
        return m_items.at(index.row()).first;
    case ValueRole:
        return m_items.at(index.row()).second;
    default:
        return QVariant();
    }
}

QMap<int, QVariant> SelectionModel::itemData(const QModelIndex &index) const {
    QMap<int, QVariant> map;
    map[NameRole] = data(index, NameRole);
    map[ValueRole] = data(index, ValueRole);
    
    return map;
}

bool SelectionModel::setData(const QModelIndex &index, const QVariant &value, int role) {
    if (!index.isValid()) {
        return false;
    }
    
    switch (role) {
    case NameRole:
        m_items[index.row()].first = value.toString();
        break;
    case ValueRole:
        m_items[index.row()].second = value;
        break;
    default:
        return false;
    }
    
    return true;
}

bool SelectionModel::setItemData(const QModelIndex &index, const QMap<int, QVariant> &roles) {
    QMapIterator<int, QVariant> iterator(roles);
    bool ok = false;
    
    while (iterator.hasNext()) {
        iterator.next();
        ok = setData(index, iterator.value(), iterator.key());
        
        if (!ok) {
            return false;
        }
    }
    
    return ok;
}

QVariant SelectionModel::data(int row, const QByteArray &role) const {
    return data(index(row), m_roles.key(role));
}

QVariantMap SelectionModel::itemData(int row) const {
    QVariantMap map;
    map["name"] = data(row, "name");
    map["value"] = data(row, "value");
    
    return map;
}

bool SelectionModel::setData(int row, const QVariant &value, const QByteArray &role) {
    return setData(index(row), value, m_roles.key(role));
}

bool SelectionModel::setItemData(int row, const QVariantMap &roles) {
    QMapIterator<QString, QVariant> iterator(roles);
    bool ok = false;
    
    while (iterator.hasNext()) {
        iterator.next();
        ok = setData(row, iterator.value(), iterator.key().toUtf8());
        if (!ok) {
            return false;
        }
    }
    
    return ok;
}

int SelectionModel::match(const QByteArray &role, const QVariant &value) const {
    for (int i = 0; i < m_items.size(); i++) {
        if (data(i, role) == value) {
            return i;
        }
    }
    
    return -1;
}

void SelectionModel::append(const QString &name, const QVariant &value) {
    beginInsertRows(QModelIndex(), m_items.size(), m_items.size());
    m_items << QPair<QString, QVariant>(name, value);
    endInsertRows();
    emit countChanged(rowCount());
}

void SelectionModel::insert(int row, const QString &name, const QVariant &value) {
    if ((row < 0) || (row >= m_items.size())) {
        append(name, value);
    }
    else {
        beginInsertRows(QModelIndex(), row, row);
        m_items.insert(row, QPair<QString, QVariant>(name, value));
        endInsertRows();
        emit countChanged(rowCount());
    }
}

bool SelectionModel::remove(int row) {
    if ((row >= 0) && (row < m_items.size())) {
        beginRemoveRows(QModelIndex(), row, row);
        m_items.removeAt(row);
        endRemoveRows();
        emit countChanged(rowCount());
        
        return true;
    }
    
    return false;
}

void SelectionModel::clear() {
    if (!m_items.isEmpty()) {
        beginResetModel();
        m_items.clear();
        endResetModel();
        emit countChanged(0);
    }
}
