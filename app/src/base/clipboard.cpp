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

#include "clipboard.h"
#include "settings.h"
#include <QApplication>
#include <QClipboard>
#ifdef MEEGO_EDITION_HARMATTAN
#include <QTimer>
#endif
#ifdef CUTETUBE_DEBUG
#include <QDebug>
#endif

Clipboard* Clipboard::self = 0;

Clipboard::Clipboard(QObject *parent) :
    QObject(parent),
    m_monitor(false)
#ifdef MEEGO_EDITION_HARMATTAN
    ,m_timer(0)
#endif
{
    if (!self) {
        self = this;
    }
    
    connect(Settings::instance(), SIGNAL(clipboardMonitorEnabledChanged()), this, SLOT(onMonitorEnabledChanged()));
    onMonitorEnabledChanged();
}

Clipboard::~Clipboard() {
    if (self == this) {
        self = 0;
    }
}

Clipboard* Clipboard::instance() {
    return self;
}

QString Clipboard::text() const {
    return QApplication::clipboard()->text();
}

void Clipboard::setText(const QString &text) {
    disconnect(QApplication::clipboard(), SIGNAL(dataChanged()), this, SLOT(onTextChanged()));
    QApplication::clipboard()->setText(text);
    
    if (m_monitor) {
        connect(QApplication::clipboard(), SIGNAL(dataChanged()), this, SLOT(onTextChanged()));
    }
}

void Clipboard::onMonitorEnabledChanged() {
    if (Settings::instance()->clipboardMonitorEnabled()) {
        m_monitor = true;
        connect(QApplication::clipboard(), SIGNAL(dataChanged()), this, SLOT(onTextChanged()));
    }
    else {
        m_monitor = false;
        disconnect(QApplication::clipboard(), SIGNAL(dataChanged()), this, SLOT(onTextChanged()));
    }
#ifdef CUTETUBE_DEBUG
    qDebug() << "Clipboard::onMonitorEnabledChanged" << m_monitor;
#endif
}

void Clipboard::onTextChanged() {
#ifdef MEEGO_EDITION_HARMATTAN
    if ((m_timer) && (m_timer->isActive())) {
        // QClipboard::dataChanged() signal is emitted twice in Harmattan,
        // so ignore the signal if the timer is still active.
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
#ifdef CUTETUBE_DEBUG
    qDebug() << "Clipboard::onTextChanged" << text;
#endif
    if (!text.isEmpty()) {
        emit textChanged(text);
    }
}
