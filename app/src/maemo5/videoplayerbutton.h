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

#ifndef VIDEOPLAYERBUTTON_H
#define VIDEOPLAYERBUTTON_H

#include <QAbstractButton>

class VideoPlayerButton : public QAbstractButton
{
    Q_OBJECT

public:
    enum Role {
        PlayPause = 0,
        Previous,
        Next,
        Back,
        Settings
    };

public:
    explicit VideoPlayerButton(Role buttonRole, QWidget *parent = 0);

protected:
    void paintEvent(QPaintEvent *);

private:
    Role m_role;
};

#endif // VIDEOPLAYERBUTTON_H
