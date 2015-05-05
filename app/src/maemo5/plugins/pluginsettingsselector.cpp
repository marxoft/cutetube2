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

#include "pluginsettingsselector.h"
#include "selectionmodel.h"
#include <QMaemo5ListPickSelector>
#include <QSettings>

PluginSettingsSelector::PluginSettingsSelector(const QString &text, QWidget *parent) :
    ValueSelector(text, parent)
{
    setModel(new SelectionModel(this));
}

void PluginSettingsSelector::setKey(const QString &key) {
    m_key = key;
}

void PluginSettingsSelector::setDefaultValue(const QVariant &value) {
    m_default = value;
}

void PluginSettingsSelector::load() {
    if (!model()) {
        return;
    }

    QVariant value = QSettings().value(key(), defaultValue());

    bool found = false;
    int i = 0;

    while ((!found) && (i < model()->rowCount())) {
        found = model()->data(model()->index(i, 0), SelectionModel::ValueRole) == value;

        if (found) {
            m_selector->setCurrentIndex(i);
        }

        i++;
    }

    if (!found) {
        m_selector->setCurrentIndex(0);
    }
}

void PluginSettingsSelector::onSelected() {
    if (!key().isEmpty()) {
        QSettings().setValue(key(), currentValue());
    }
}
