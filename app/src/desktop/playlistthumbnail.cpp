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

#include "playlistthumbnail.h"
#include <QPainter>

PlaylistThumbnail::PlaylistThumbnail(QWidget *parent) :
    Image(parent),
    m_count(0)
{
}

int PlaylistThumbnail::videoCount() const {
    return m_count;
}

void PlaylistThumbnail::setVideoCount(int count) {
    if (count != videoCount()) {
        m_count = count;
        update();
    }
}

void PlaylistThumbnail::paintEvent(QPaintEvent *e) {
    Image::paintEvent(e);
    
    QPainter painter(this);
    painter.setPen(palette().color(QPalette::Mid));
    
    QRect border = rect();
    border.setRight(border.right() - 1);
    border.setBottom(border.bottom() - 1);
    painter.drawRect(border);
    
    QFont font;
    QRect countRect = border;
    const int countHeight = countRect.height() / 4;
    font.setPixelSize(countHeight);
    countRect.setLeft(countRect.left() + 1);
    countRect.setTop(countRect.bottom() - (countHeight));

    painter.setOpacity(0.8);
    painter.fillRect(countRect, Qt::black);
    painter.setFont(font);
    painter.setOpacity(1);
    painter.setPen(Qt::white);
    painter.drawText(countRect, Qt::AlignCenter, videoCount() > 0 ? tr("%1 videos").arg(videoCount()) : tr("No videos"));
}
