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

#include "videoplayerbutton.h"
#include <QPainter>

VideoPlayerButton::VideoPlayerButton(Role buttonRole, QWidget *parent) :
    QAbstractButton(parent),
    m_role(buttonRole)
{
    setAttribute(Qt::WA_TranslucentBackground, true);
    setStyleSheet("background:transparent");
    setFixedSize(70, 70);
    setFocusPolicy(Qt::NoFocus);
}

void VideoPlayerButton::paintEvent(QPaintEvent *) {
    QPainter painter(this);

    switch (m_role) {
    case Previous:
        painter.drawImage(3, 3, isDown() ? QImage("/etc/hildon/theme/mediaplayer/BackPressed.png")
                                         : QImage("/etc/hildon/theme/mediaplayer/Back.png"));
        break;
    case Next:
        painter.drawImage(3, 3, isDown() ? QImage("/etc/hildon/theme/mediaplayer/ForwardPressed.png")
                                         : QImage("/etc/hildon/theme/mediaplayer/Forward.png"));
        break;
    default:
        icon().paint(&painter, 3, 3, 64, 64);
        break;
    }
}
