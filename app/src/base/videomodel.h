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

#ifndef VIDEOMODEL_H
#define VIDEOMODEL_H

#include "video.h"
#include <QAbstractListModel>

class VideoModel : public QAbstractListModel
{
    Q_OBJECT
    
    Q_PROPERTY(int count READ rowCount NOTIFY countChanged)
    
public:
    enum Roles {
        DateRole = Qt::UserRole + 1,
        DescriptionRole,
        DurationRole,
        IdRole,
        LargeThumbnailUrlRole,
        ServiceRole,
        StreamUrlRole,
        ThumbnailUrlRole,
        TitleRole,
        UrlRole,
        ViewCountRole
    };
    
    explicit VideoModel(QObject *parent = 0);
    
#if QT_VERSION >= 0x050000
    QHash<int, QByteArray> roleNames() const;
#endif
    int rowCount(const QModelIndex &parent = QModelIndex()) const;
        
    QVariant data(const QModelIndex &index, int role) const;
    QMap<int, QVariant> itemData(const QModelIndex &index) const;
    
    Q_INVOKABLE QVariant data(int row, const QByteArray &role) const;
    Q_INVOKABLE QVariantMap itemData(int row) const;
    
    Q_INVOKABLE CTVideo* get(int row) const;
    Q_INVOKABLE void append(CTVideo *video);
    Q_INVOKABLE void insert(int row, CTVideo *video);
    Q_INVOKABLE void remove(int row);

public Q_SLOTS:
    void clear();
    
Q_SIGNALS:
    void countChanged(int c);
    
private:
    QList<CTVideo*> m_items;
    
    QHash<int, QByteArray> m_roles;
};
    
#endif // VIDEOMODEL_H
