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

#ifndef SEARCHHISTORYMODEL_H
#define SEARCHHISTORYMODEL_H

#include <QSortFilterProxyModel>

class QStringListModel;

class SearchHistoryModel : public QSortFilterProxyModel
{
    Q_OBJECT
    
    Q_PROPERTY(int count READ rowCount NOTIFY countChanged)
    Q_PROPERTY(Qt::Alignment textAlignment READ textAlignment WRITE setTextAlignment NOTIFY textAlignmentChanged)

public:
    explicit SearchHistoryModel(QObject *parent = 0);

    Qt::Alignment textAlignment() const;
    void setTextAlignment(Qt::Alignment align);

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
    Q_INVOKABLE QVariant data(int row, const QByteArray &role) const;
            
    Q_INVOKABLE void addSearch(const QString &query);
    Q_INVOKABLE void removeSearch(const QString &query);
    Q_INVOKABLE void removeSearch(int row);

public Q_SLOTS:
    void clear();
    void reload();
    
Q_SIGNALS:
    void countChanged(int c);
    void textAlignmentChanged();
    
private:
    QStringListModel *m_model;

    Qt::Alignment m_alignment;
};
    
#endif // SEARCHHISTORYMODEL_H
