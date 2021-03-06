/*
 * Copyright (C) 2015 Stuart Howarth <showarth@marxoft.co.uk>
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

#include "accountdelegate.h"
#include <QPainter>

AccountDelegate::AccountDelegate(int activeRole, QObject *parent) :
    QStyledItemDelegate(parent),
    m_activeRole(activeRole)
{
}

void AccountDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const {
    if ((option.state) & (QStyle::State_Selected)) {
        painter->drawImage(option.rect, QImage("/etc/hildon/theme/images/TouchListBackgroundPressed.png"));
    }
    else {
        painter->drawImage(option.rect, QImage("/etc/hildon/theme/images/TouchListBackgroundNormal.png"));
    }
    
    QRect textRect = option.rect;
    textRect.moveLeft(textRect.left() + 8);

    if (index.data(m_activeRole).toBool()) {
        QRect iconRect = option.rect;
        iconRect.moveLeft(iconRect.right() - 56);
        iconRect.moveTop(iconRect.top() + (iconRect.height() - 48) / 2);
        iconRect.setSize(QSize(48, 48));
        painter->drawImage(iconRect, QImage("/usr/share/icons/hicolor/48x48/hildon/widgets_tickmark_grid.png"));
        
        textRect.setRight(iconRect.left() - 8);    
    }
    else {
        textRect.setRight(textRect.right() - 8);
    }
    
    const QString text = painter->fontMetrics().elidedText(index.data(Qt::DisplayRole).toString(), Qt::ElideRight,
                                                           textRect.width());
    painter->drawText(textRect, Qt::AlignLeft | Qt::AlignVCenter, text);
}
