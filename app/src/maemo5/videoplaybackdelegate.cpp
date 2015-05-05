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

#include "videoplaybackdelegate.h"
#include "videomodel.h"
#include "imagecache.h"
#include <QPainter>
#include <QUrl>
#include <QApplication>

VideoPlaybackDelegate::VideoPlaybackDelegate(ImageCache *cache, QObject *parent) :
    QStyledItemDelegate(parent),
    m_cache(cache)
{
}

void VideoPlaybackDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option,
                                  const QModelIndex &index) const {
    
    if ((option.state) & (QStyle::State_Selected)) {
        painter->drawImage(option.rect, QImage("/etc/hildon/theme/images/TouchListBackgroundPressed.png"));
    }
    
    QUrl imageUrl = index.data(VideoModel::ThumbnailUrlRole).toUrl();
    QImage image = m_cache->image(imageUrl, QSize(80, 60));
    
    QRect imageRect = option.rect;
    imageRect.setLeft(imageRect.left() + 5);
    imageRect.setTop(imageRect.top() + 5);
    imageRect.setWidth(80);
    imageRect.setHeight(60);
    
    if (!image.isNull()) {
        painter->drawImage(imageRect, image);
    }
    else {
        painter->fillRect(imageRect, Qt::black);
    }
    
    QString duration = index.data(VideoModel::DurationRole).toString();
    QFont font;
    QRect durationRect = imageRect;
    const int durationHeight = durationRect.height() / 4;
    font.setPixelSize(durationHeight);        
    durationRect.setTop(durationRect.bottom() - (durationHeight));
    durationRect.setLeft(durationRect.right() - QFontMetrics(font).width(duration));

    painter->save();
    painter->setOpacity(0.8);
    painter->fillRect(durationRect, Qt::black);
    painter->setFont(font);
    painter->setOpacity(1);
    painter->setPen(Qt::white);
    painter->drawText(durationRect, Qt::AlignCenter, duration);
    painter->restore();
    
    painter->save();
    painter->setPen(QApplication::palette().color(QPalette::Mid));
    painter->drawRect(imageRect);
    painter->setPen(QApplication::palette().color(QPalette::Text));
    
    QRect textRect = option.rect;
    textRect.setLeft(textRect.left() + 90);
    textRect.setRight(textRect.right() - 5);
    
    font.setPixelSize(18);
    painter->setFont(font);
    painter->drawText(textRect, Qt::AlignVCenter | Qt::TextWordWrap, index.data(VideoModel::TitleRole).toString());
    painter->restore();
}

QSize VideoPlaybackDelegate::sizeHint(const QStyleOptionViewItem &option, const QModelIndex &) const {
    return QSize(option.rect.width(), 70);
}
