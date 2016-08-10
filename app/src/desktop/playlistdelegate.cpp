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

#include "playlistdelegate.h"
#include "drawing.h"
#include "imagecache.h"
#include <QPainter>
#include <QApplication>

PlaylistDelegate::PlaylistDelegate(ImageCache *cache, int dateRole, int thumbnailRole, int titleRole, int usernameRole,
                                   int videoCountRole, QObject *parent) :
    QStyledItemDelegate(parent),
    m_cache(cache),
    m_dateRole(dateRole),
    m_thumbnailRole(thumbnailRole),
    m_titleRole(titleRole),
    m_usernameRole(usernameRole),
    m_videoCountRole(videoCountRole)
{
}

void PlaylistDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const {
    QStyledItemDelegate::paint(painter, option, index);
    const QImage image = m_cache->image(index.data(m_thumbnailRole).toString(), QSize(80, 60));
    
    QRect imageRect = option.rect;
    imageRect.setLeft(imageRect.left() + 8);
    imageRect.setTop(imageRect.top() + 8);
    imageRect.setWidth(80);
    imageRect.setHeight(60);
    
    painter->fillRect(imageRect, Qt::black);
    drawCenteredImage(painter, imageRect, image);
    
    const int count = index.data(m_videoCountRole).toInt();
    
    if (count != -1) {
        QFont font;
        QRect durationRect = imageRect;
        const int durationHeight = durationRect.height() / 4;
        font.setPixelSize(durationHeight);        
        durationRect.setTop(durationRect.bottom() - (durationHeight));
        
        painter->save();
        painter->setOpacity(0.8);
        painter->fillRect(durationRect, Qt::black);
        painter->setFont(font);
        painter->setOpacity(1);
        painter->setPen(Qt::white);
        painter->drawText(durationRect, Qt::AlignCenter, count > 0 ? tr("%1 videos").arg(count) : tr("No videos"));
        painter->restore();
    }
    
    painter->save();
    painter->setPen(QApplication::palette().color(QPalette::Mid));
    painter->drawRect(imageRect);
    painter->setPen(QApplication::palette().color(QPalette::Text));
    
    QRect textRect = option.rect;
    textRect.setLeft(imageRect.right() + 8);
    textRect.setRight(textRect.right() - 8);
    textRect.setTop(textRect.top() + 8);
    textRect.setBottom(textRect.bottom() - 8);
    
    QString subTitle;
    const QString username = index.data(m_usernameRole).toString();
    const QString date = index.data(m_dateRole).toString();
    
    if (!username.isEmpty()) {
        subTitle.append(QString("%1 %2").arg(tr("by")).arg(username));
    }
    
    if (!date.isEmpty()) {
        if (!username.isEmpty()) {
            subTitle.append(QString(" %1 ").arg(tr("on")));
        }
        
        subTitle.append(date);
    }
    
    if (!subTitle.isEmpty()) {
        painter->drawText(textRect, Qt::AlignLeft | Qt::AlignTop,
                          painter->fontMetrics().elidedText(index.data(m_titleRole).toString(),
                          Qt::ElideRight, textRect.width()));
        painter->setPen(QApplication::palette().color(QPalette::Mid));
        painter->drawText(textRect, Qt::AlignLeft | Qt::AlignBottom,
                          painter->fontMetrics().elidedText(subTitle, Qt::ElideRight, textRect.width()));
    }
    else {                
        painter->drawText(textRect, Qt::AlignVCenter | Qt::TextWordWrap, index.data(m_titleRole).toString());
    }
    
    painter->restore();
}

QSize PlaylistDelegate::sizeHint(const QStyleOptionViewItem &option, const QModelIndex &) const {
    return QSize(option.rect.width(), 76);
}
