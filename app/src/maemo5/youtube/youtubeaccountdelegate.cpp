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

#include "youtubeaccountdelegate.h"
#include "youtubeaccountmodel.h"
#include <QPainter>

YouTubeAccountDelegate::YouTubeAccountDelegate(QObject *parent) :
    QStyledItemDelegate(parent)
{
}

void YouTubeAccountDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option,
                                   const QModelIndex &index) const {
    
    QStyledItemDelegate::paint(painter, option, index);

    if (index.data(YouTubeAccountModel::ActiveRole).toBool()) {
        QRect checkRect = option.rect;
        checkRect.moveLeft(checkRect.right() - 53);
        checkRect.moveTop(checkRect.top() + 16);
        checkRect.setSize(QSize(48, 48));
        painter->drawImage(checkRect, QImage("/usr/share/icons/hicolor/48x48/hildon/widgets_tickmark_grid.png"));
    }
}
