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

#ifndef CLIPBOARD_H
#define CLIPBOARD_H

#include <QObject>

class Clipboard : public QObject
{
    Q_OBJECT

    Q_PROPERTY(bool enabled READ isEnabled WRITE setEnabled NOTIFY enabledChanged)
    Q_PROPERTY(QString text READ text WRITE setText NOTIFY textChanged)

public:
    ~Clipboard();
    
    static Clipboard* instance();
    
    static QString text();

    bool isEnabled() const;
    
public Q_SLOTS:    
    static void setText(const QString &text);

    void setEnabled(bool enabled);

private Q_SLOTS:
    void onTextChanged();
    
Q_SIGNALS:
    void enabledChanged(bool enabled);
    void textChanged(const QString &text);

private:
    Clipboard();
    
    static Clipboard *self;

    bool m_enabled;
};

#endif // CLIPBOARD_H
