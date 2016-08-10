/*
 * Copyright (C) 2016 Stuart Howarth <showarth@marxoft.co.uk>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 3 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef PLUGINSETTINGSTAB_H
#define PLUGINSETTINGSTAB_H

#include "settingstab.h"
#include <QVariantMap>

class PluginSettings;
class QFormLayout;

class PluginSettingsTab : public SettingsTab
{
    Q_OBJECT

public:
    explicit PluginSettingsTab(const QString &pluginId, const QVariantList &settings, QWidget *parent = 0);

public Q_SLOTS:
    virtual void save();

private Q_SLOTS:
    void setBooleanValue(bool value);
    void setIntegerValue(int value);
    void setListValue(int value);
    void setTextValue(const QString &value);

private:
    void addCheckBox(QFormLayout *layout, const QString &label, const QString &key, bool value);
    void addComboBox(QFormLayout *layout, const QString &label, const QString &key, const QVariantList &options,
                     const QVariant &value);
    void addGroupBox(QFormLayout *layout, const QString &label, const QString &key, const QVariantList &settings);
    void addLineEdit(QFormLayout *layout, const QString &label, const QString &key, const QString &value,
                     bool isPassword = false);
    void addSpinBox(QFormLayout *layout, const QString &label, const QString &key, int minimum, int maximum,
                    int step, int value);
    void addWidget(QFormLayout *layout, const QVariantMap &setting, const QString &group = QString());

    PluginSettings *m_plugin;
    
    QFormLayout *m_layout;

    QVariantMap m_settings;
};

#endif // PLUGINSETTINGSTAB_H
