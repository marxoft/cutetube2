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

#ifndef CLIPBOARD_H
#define CLIPBOARD_H

#include <QObject>
#include <qplatformdefs.h>

#ifdef MEEGO_EDITION_HARMATTAN
class QTimer;
#endif

class Clipboard : public QObject
{
    Q_OBJECT
    
    Q_PROPERTY(QString text READ text WRITE setText NOTIFY textChanged)

public:
    explicit Clipboard(QObject *parent = 0);
    ~Clipboard();
    
    static Clipboard* instance();
    
    QString text() const;
    
public Q_SLOTS:
    void setText(const QString &text);

private Q_SLOTS:
    void onMonitorEnabledChanged();
    void onTextChanged();
    
Q_SIGNALS:
    void textChanged(const QString &text);

private:
    static Clipboard *self;
    
    bool m_monitor;
#ifdef MEEGO_EDITION_HARMATTAN
    QTimer *m_timer;
#endif
};

#endif // CLIPBOARD_H
