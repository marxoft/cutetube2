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

#include "clipboard.h"
#include "logger.h"
#include <QApplication>
#include <QClipboard>

Clipboard* Clipboard::self = 0;

Clipboard::Clipboard() :
    QObject(),
    m_enabled(false)
{
}

Clipboard::~Clipboard() {
    self = 0;
}

Clipboard* Clipboard::instance() {
    return self ? self : self = new Clipboard;
}

QString Clipboard::text() {
    return QApplication::clipboard()->text();
}

void Clipboard::setText(const QString &text) {
    if (self) {
        self->disconnect(QApplication::clipboard(), SIGNAL(dataChanged()), self, SLOT(onTextChanged()));
    }
    
    QApplication::clipboard()->setText(text);
    
    if ((self) && (self->isEnabled())) {
        self->connect(QApplication::clipboard(), SIGNAL(dataChanged()), self, SLOT(onTextChanged()));
    }
}

bool Clipboard::isEnabled() const {
    return m_enabled;
}

void Clipboard::setEnabled(bool enabled) {
    if (enabled != isEnabled()) {
        m_enabled = enabled;
        emit enabledChanged(enabled);
        
        if (enabled) {
            connect(QApplication::clipboard(), SIGNAL(dataChanged()), this, SLOT(onTextChanged()));
        }
        else {
            disconnect(QApplication::clipboard(), SIGNAL(dataChanged()), this, SLOT(onTextChanged()));
        }
    }
}

void Clipboard::onTextChanged() {
    const QString text = QApplication::clipboard()->text();
    Logger::log("Clipboard::onTextChanged(). Text: " + text, Logger::HighVerbosity);

    if (!text.isEmpty()) {
        emit textChanged(text);
    }
}
