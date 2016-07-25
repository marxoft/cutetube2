/*
 * Copyright (C) 2016 Stuart Howarth <showarth@marxoft.co.uk>
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms and conditions of the GNU General Public License,
 * version 3, as published by the Free Software Foundation.
 *
 * This program is distributed in the hope it will be useful, but WITHOUT ANY
 * WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software Foundation,
 * Inc., 51 Franklin St - Fifth Floor, Boston, MA 02110-1301 USA.
 */

#include "screensaver.h"

ScreenSaver::ScreenSaver(QObject *parent) :
    QObject(parent)
{
    m_timer.setInterval(60000);
    connect(&m_timer, SIGNAL(timeout()), this, SLOT(preventBlanking()));
}

void ScreenSaver::preventBlanking(bool prevent) {
    if (prevent) {
        m_displayState.setBlankingPause();
        m_timer.start();
    }
    else {
        m_displayState.cancelBlankingPause();
        m_timer.stop();
    }
}
