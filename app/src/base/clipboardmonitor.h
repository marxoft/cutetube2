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

#ifndef CLIPBOARDMONITOR_H
#define CLIPBOARDMONITOR_H

#include <QObject>
#include <qplatformdefs.h>

#ifdef MEEGO_EDITION_HARMATTAN
class QTimer;
#endif

class ClipboardMonitor : public QObject
{
    Q_OBJECT

public:
    explicit ClipboardMonitor(QObject *parent = 0);
    ~ClipboardMonitor();
    
    static ClipboardMonitor* instance();

private Q_SLOTS:
    void onClipboardMonitorEnabledChanged();
    void onClipboardTextChanged();

private:
    static ClipboardMonitor* self;
#ifdef MEEGO_EDITION_HARMATTAN
    QTimer *m_timer;
#endif
};

#endif // CLIPBOARDMONITOR_H
