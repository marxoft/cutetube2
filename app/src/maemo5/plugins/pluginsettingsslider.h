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

#ifndef PLUGINSETTINGSSLIDER_H
#define PLUGINSETTINGSSLIDER_H

#include <QSlider>
#include <QVariant>

class PluginSettingsSlider : public QSlider
{
    Q_OBJECT

public:
    explicit PluginSettingsSlider(QWidget *parent = 0);

    inline QString key() const { return m_key; }
    inline QVariant defaultValue() const { return m_default; }
    
public Q_SLOTS:
    void setKey(const QString &key);
    void setDefaultValue(const QVariant &value);
    void load();

private Q_SLOTS:
    void onReleased();

private:
    QString m_key;
    QVariant m_default;
};

#endif // PLUGINSETTINGSSLIDER_H
