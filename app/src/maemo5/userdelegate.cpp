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

#include "userdelegate.h"
#include "drawing.h"
#include "imagecache.h"
#include "utils.h"
#include <QApplication>
#include <QPainter>

UserDelegate::UserDelegate(ImageCache *cache, int subscriberCountRole, int thumbnailRole, int usernameRole,
                           QObject *parent) :
    QStyledItemDelegate(parent),
    m_cache(cache),
    m_subscriberCountRole(subscriberCountRole),
    m_thumbnailRole(thumbnailRole),
    m_usernameRole(usernameRole)
{
    m_smallFont.setPointSize(13);
}

void UserDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const {    
    if ((option.state) & (QStyle::State_Selected)) {
        painter->drawImage(option.rect, QImage("/etc/hildon/theme/images/TouchListBackgroundPressed.png"));
    }
    else {
        painter->drawImage(option.rect, QImage("/etc/hildon/theme/images/TouchListBackgroundNormal.png"));
    }
    
    const QImage image = m_cache->image(index.data(m_thumbnailRole).toString(), QSize(75, 75));
    
    QRect imageRect = option.rect;
    imageRect.setLeft(imageRect.left() + 8);
    imageRect.setTop(imageRect.top() + 8);
    imageRect.setWidth(75);
    imageRect.setHeight(75);
    
    drawCenteredImage(painter, imageRect, image);
    
    QRect textRect = option.rect;
    textRect.setLeft(imageRect.right() + 8);
    textRect.setRight(textRect.right() - 8);
    textRect.setTop(textRect.top() + 8);
    textRect.setBottom(textRect.bottom() - 8);
    
    painter->drawText(textRect, Qt::AlignLeft | Qt::AlignTop,
                          painter->fontMetrics().elidedText(index.data(m_usernameRole).toString(),
                          Qt::ElideRight, textRect.width()));
    painter->save();
    
    const int count = index.data(m_subscriberCountRole).toInt();
    
    painter->setFont(m_smallFont);
    painter->setPen(QApplication::palette().color(QPalette::Mid));
    painter->drawText(textRect, Qt::AlignLeft | Qt::AlignBottom,
                      count > 0 ? tr("%1 subscribers").arg(Utils::formatLargeNumber(count)) : tr("No subscribers"));    
    painter->restore();
}

QSize UserDelegate::sizeHint(const QStyleOptionViewItem &option, const QModelIndex &) const {
    return QSize(option.rect.width(), 91);
}
