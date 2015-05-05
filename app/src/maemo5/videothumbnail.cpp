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

#include "videothumbnail.h"
#include <QPainter>

VideoThumbnail::VideoThumbnail(QWidget *parent) :
    Image(parent)
{
}

QString VideoThumbnail::duration() const {
    return m_duration;
}

void VideoThumbnail::setDuration(const QString &d) {
    if (d != duration()) {
        m_duration = d;
        update();
    }
}

void VideoThumbnail::paintEvent(QPaintEvent *e) {
    Image::paintEvent(e);
    
    QPainter painter(this);
    painter.setPen(palette().color(QPalette::Mid));
    
    QRect border = rect();
    border.setRight(border.right() - 1);
    border.setBottom(border.bottom() - 1);
    painter.drawRect(border);
    
    if (duration().isEmpty()) {
        return;
    }
    
    QFont font;
    QRect durationRect = border;
    const int durationHeight = durationRect.height() / 4;
    font.setPixelSize(durationHeight);        
    durationRect.setTop(durationRect.bottom() - (durationHeight));
    durationRect.setLeft(durationRect.right() - QFontMetrics(font).width(duration()));

    painter.setOpacity(0.8);
    painter.fillRect(durationRect, Qt::black);
    painter.setFont(font);
    painter.setOpacity(1);
    painter.setPen(Qt::white);
    painter.drawText(durationRect, Qt::AlignCenter, duration());
}
