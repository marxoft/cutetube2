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

#ifndef SELECTIONMODEL_H
#define SELECTIONMODEL_H

#include <QAbstractListModel>

class SelectionModel : public QAbstractListModel
{
    Q_OBJECT
    
    Q_PROPERTY(int count READ rowCount NOTIFY countChanged)
    Q_PROPERTY(Qt::Alignment textAlignment READ textAlignment WRITE setTextAlignment NOTIFY textAlignmentChanged)
    
public:
    enum Roles {
        NameRole = Qt::DisplayRole,
        ValueRole = Qt::UserRole
    };
    
    explicit SelectionModel(QObject *parent = 0);
        
#if QT_VERSION >= 0x050000
    QHash<int, QByteArray> roleNames() const;
#endif
    
    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    
    Qt::Alignment textAlignment() const;
    void setTextAlignment(Qt::Alignment align);
    
    QVariant data(const QModelIndex &index, int role = NameRole) const;
    QMap<int, QVariant> itemData(const QModelIndex &index) const;
    bool setData(const QModelIndex &index, const QVariant &value, int role);
    bool setItemData(const QModelIndex &index, const QMap<int, QVariant> &roles);
    
    Q_INVOKABLE QVariant data(int row, const QByteArray &role) const;
    Q_INVOKABLE QVariantMap itemData(int row) const;
    Q_INVOKABLE bool setData(int row, const QVariant &value, const QByteArray &role);
    Q_INVOKABLE bool setItemData(int row, const QVariantMap &roles);
    
    Q_INVOKABLE int match(const QByteArray &role, const QVariant &value) const;
    
    Q_INVOKABLE void append(const QString &name, const QVariant &value);
    Q_INVOKABLE void insert(int row, const QString &name, const QVariant &value);
    Q_INVOKABLE bool remove(int row);

public Q_SLOTS:
    void clear();
    
Q_SIGNALS:
    void countChanged(int c);
    void textAlignmentChanged();
    
protected:
    QList< QPair<QString, QVariant> > m_items;
    
    QHash<int, QByteArray> m_roles;
    
    Qt::Alignment m_alignment;
};

#endif // SELECTIONMODEL_H
