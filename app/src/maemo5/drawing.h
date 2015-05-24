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

#ifndef DRAWING_H
#define DRAWING_H

#include <QPainter>

inline void drawCenteredImage(QPainter *painter, const QRect &rect, const QImage &image) {
    painter->save();
    painter->setClipRect(rect);
    
    const int wDiff = rect.width() - image.width();
    const int hDiff = rect.height() - image.height();
    const int x = rect.x() + (wDiff == 0 ? 0 : wDiff / 2);
    const int y = rect.y() + (hDiff == 0 ? 0 : hDiff / 2);
    
    painter->drawImage(x, y, image);
    painter->restore();
}

inline void drawCenteredImage(QPainter *painter, const QRect &rect, const QPixmap &pixmap) {
    painter->save();
    painter->setClipRect(rect);
    
    const int wDiff = rect.width() - pixmap.width();
    const int hDiff = rect.height() - pixmap.height();
    const int x = rect.x() + (wDiff == 0 ? 0 : wDiff / 2);
    const int y = rect.y() + (hDiff == 0 ? 0 : hDiff / 2);
    
    painter->drawPixmap(x, y, pixmap);
    painter->restore();
}

#endif // DRAWING_H
