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

#include "pluginsettingsspinbox.h"
#include <QSettings>

PluginSettingsSpinbox::PluginSettingsSpinbox(QWidget *parent) :
    QSpinBox(parent)
{
    connect(this, SIGNAL(editingFinished()), this, SLOT(onValueEdited()));
}

void PluginSettingsSpinbox::setKey(const QString &key) {
    m_key = key;
}

void PluginSettingsSpinbox::setDefaultValue(const QVariant &value) {
    m_default = value;
}

void PluginSettingsSpinbox::load() {
    int value = QSettings().value(key(), defaultValue()).toInt();
    setValue(value);
}

void PluginSettingsSpinbox::onValueEdited() {
    if (!key().isEmpty()) {
        QSettings().setValue(key(), value());
    }
}
