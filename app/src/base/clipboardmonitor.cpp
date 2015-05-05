/*
 * Copyright (C) 2014 Stuart Howarth <showarth@marxoft.co.uk>
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms and conditions of the GNU Lesser General Public License,
 * version 3, as published by the Free Software Foundation.
 *
 * This program is distributed in the hope it will be useful, but WITHOUT ANY
 * WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for
 * more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program; if not, write to the Free Software Foundation,
 * Inc., 51 Franklin St - Fifth Floor, Boston, MA 02110-1301 USA.
 */

#include "clipboardmonitor.h"
#include "mainwindow.h"
#include "settings.h"
#include <QApplication>
#include <QClipboard>
#ifdef MEEGO_EDITION_HARMATTAN
#include <QTimer>
#endif

ClipboardMonitor* ClipboardMonitor::self = 0;

ClipboardMonitor::ClipboardMonitor(QObject *parent) :
    QObject(parent)
#ifdef MEEGO_EDITION_HARMATTAN
    ,m_timer(0)
#endif
{
    if (!self) {
        self = this;
    }

    connect(Settings::instance(), SIGNAL(clipboardMonitorEnabledChanged()),
            this, SLOT(onClipboardMonitorEnabledChanged()));
    onClipboardMonitorEnabledChanged();
}

ClipboardMonitor::~ClipboardMonitor() {
    if (self == this) {
        self = 0;
    }
}

ClipboardMonitor* ClipboardMonitor::instance() {
    return self;
}

void ClipboardMonitor::onClipboardMonitorEnabledChanged() {
    if (Settings::instance()->clipboardMonitorEnabled()) {
        connect(QApplication::clipboard(), SIGNAL(dataChanged()), this, SLOT(onClipboardTextChanged()));
    }
    else {
        disconnect(QApplication::clipboard(), SIGNAL(dataChanged()), this, SLOT(onClipboardTextChanged()));
    }
}

void ClipboardMonitor::onClipboardTextChanged() {
#ifdef MEEGO_EDITION_HARMATTAN
    if ((m_timer) && (m_timer->isActive())) {
        // QClipboard::dataChanged() signal is emitted twice in Harmattan, so ignore the signal if the timer is still active.
        return;
    }
    else {
        if (!m_timer) {
            m_timer = new QTimer(this);
            m_timer->setInterval(3000);
            m_timer->setSingleShot(true);
        }

        m_timer->start();
    }
#endif
    QString text = QApplication::clipboard()->text();
    
    if (!text.isEmpty()) {
        if (MainWindow *window = MainWindow::instance()) {
            window->showResource(text);
        }
    }
}
