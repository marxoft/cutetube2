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

#include "navdelegate.h"
#include <QPainter>

NavDelegate::NavDelegate(QObject *parent) :
    QStyledItemDelegate(parent)
{
}

void NavDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const {
    QStyledItemDelegate::paint(painter, option, index);
    
    QRect iconRect = option.rect;
    iconRect.moveLeft(iconRect.right() - 56);
    iconRect.moveTop(iconRect.top() + (iconRect.height() - 48) / 2);
    iconRect.setSize(QSize(48, 48));
    painter->drawImage(iconRect, QImage("/usr/share/icons/hicolor/48x48/hildon/general_forward.png"));
}
