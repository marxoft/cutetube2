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

#include "banner.h"
#include "drawing.h"
#include "imagecache.h"
#include <QPainter>

Banner::Banner(QWidget *parent) :
    Image(parent)
{
    setAspectRatioMode(Qt::IgnoreAspectRatio);
}

QUrl Banner::avatarSource() const {
    return m_avatarSource;
}

void Banner::setAvatarSource(const QUrl &url) {
    m_avatarSource = url;
    update();
}

void Banner::paintEvent(QPaintEvent *e) {
    Image::paintEvent(e);
    
    if (!avatarSource().isValid()) {
        return;
    }
    
    QPainter painter(this);
    QRect avatarRect = rect();
    avatarRect.setLeft(avatarRect.left() + 5);
    avatarRect.setHeight(avatarRect.height() / 2);
    avatarRect.setWidth(avatarRect.height());

    QImage avatar = m_cache->image(avatarSource(), avatarRect.size(), aspectRatioMode(), transformationMode());
    
    if (!avatar.isNull()) {
        drawCenteredImage(&painter, avatarRect, avatar);
    }
}
