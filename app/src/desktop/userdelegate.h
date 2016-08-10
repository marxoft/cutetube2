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

#ifndef USERDELEGATE_H
#define USERDELEGATE_H

#include <QStyledItemDelegate>

class ImageCache;

class UserDelegate : public QStyledItemDelegate
{
    Q_OBJECT
    
public:
    explicit UserDelegate(ImageCache *cache, int subscriberCountRole, int thumbnailRole, int usernameRole,
                          QObject *parent = 0);
    
    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const;
    
    QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &) const;
    
private:
    ImageCache *m_cache;
    
    int m_subscriberCountRole;
    int m_thumbnailRole;
    int m_usernameRole;    
};

#endif // USERDELEGATE_H
