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

#include "pluginsettingstab.h"
#include "pluginsettings.h"
#include "selectionmodel.h"
#include <QCheckBox>
#include <QComboBox>
#include <QFormLayout>
#include <QGroupBox>
#include <QLabel>
#include <QLineEdit>
#include <QSpinBox>

PluginSettingsTab::PluginSettingsTab(const QString &pluginId, const QVariantList &settings, QWidget *parent) :
    SettingsTab(parent),
    m_plugin(new PluginSettings(pluginId, this)),
    m_layout(new QFormLayout(this))
{
    foreach (const QVariant &setting, settings) {
        addWidget(m_layout, setting.toMap());
    }
}

void PluginSettingsTab::save() {
    if (m_settings.isEmpty()) {
        return;
    }

    QMapIterator<QString, QVariant> iterator(m_settings);

    while (iterator.hasNext()) {
        iterator.next();
        m_plugin->setValue(iterator.key(), iterator.value());
    }
}

void PluginSettingsTab::addCheckBox(QFormLayout *layout, const QString &label, const QString &key, bool value) {
    QCheckBox *checkbox = new QCheckBox("&" + label, this);
    checkbox->setProperty("key", key);
    checkbox->setChecked(value);
    layout->addRow(checkbox);
    connect(checkbox, SIGNAL(toggled(bool)), this, SLOT(setBooleanValue(bool)));
}

void PluginSettingsTab::addComboBox(QFormLayout *layout, const QString &label, const QString &key,
                                    const QVariantList &options, const QVariant &value) {
    QComboBox *combobox = new QComboBox(this);
    SelectionModel *model = new SelectionModel(combobox);
    combobox->setProperty("key", key);
    combobox->setModel(model);

    foreach (const QVariant &var, options) {
        const QVariantMap option = var.toMap();
        model->append(option.value("label").toString(), option.value("value"));
    }

    combobox->setCurrentIndex(qMax(0, combobox->findData(value)));
    layout->addRow("&" + label + ":", combobox);
    connect(combobox, SIGNAL(currentIndexChanged(int)), this, SLOT(setListValue(int)));
}

void PluginSettingsTab::addGroupBox(QFormLayout *layout, const QString &label, const QString &key,
                                    const QVariantList &settings) {
    QGroupBox *groupbox = new QGroupBox("&" + label, this);
    QFormLayout *form = new QFormLayout(groupbox);

    foreach (const QVariant &setting, settings) {
        addWidget(form, setting.toMap(), key);
    }

    layout->addRow(groupbox);
}

void PluginSettingsTab::addLineEdit(QFormLayout *layout, const QString &label, const QString &key,
                                    const QString &value, bool isPassword) {
    QLineEdit *edit = new QLineEdit(value, this);
    edit->setProperty("key", key);

    if (isPassword) {
        edit->setEchoMode(QLineEdit::Password);
    }

    layout->addRow("&" + label + ":", edit);
    connect(edit, SIGNAL(textChanged(QString)), this, SLOT(setTextValue(QString)));
}

void PluginSettingsTab::addSpinBox(QFormLayout *layout, const QString &label, const QString &key, int minimum,
                                   int maximum, int step, int value) {
    QSpinBox *spinbox = new QSpinBox(this);
    spinbox->setProperty("key", key);
    spinbox->setMinimum(minimum);
    spinbox->setMaximum(maximum);
    spinbox->setSingleStep(step);
    spinbox->setValue(value);
    layout->addRow("&" + label + ":", spinbox);
    connect(spinbox, SIGNAL(valueChanged(int)), this, SLOT(setIntegerValue(int)));
}

void PluginSettingsTab::addWidget(QFormLayout *layout, const QVariantMap &setting, const QString &group) {
    QString key = setting.value("key").toString();

    if (key.isEmpty()) {
        return;
    }

    if (!group.isEmpty()) {
        key.prepend("/");
        key.prepend(group);
    }

    const QString type = setting.value("type").toString();
    const QVariant value = m_plugin->value(key, setting.value("value"));

    if (type == "boolean") {
        addCheckBox(layout, setting.value("label").toString(), key, value.toBool());
    }
    else if (type == "group") {
        addGroupBox(layout, setting.value("label").toString(), key, setting.value("settings").toList());
    }
    else if (type == "integer") {
        addSpinBox(layout, setting.value("label").toString(), key, setting.value("minimum", 0).toInt(),
                   setting.value("maximum", 100).toInt(), setting.value("step", 1).toInt(), value.toInt());
    }
    else if (type == "list") {
        addComboBox(layout, setting.value("label").toString(), key, setting.value("options").toList(), value);
    }
    else if (type == "password") {
        addLineEdit(layout, setting.value("label").toString(), key, value.toString(), true);
    }
    else if (type == "text") {
        addLineEdit(layout, setting.value("label").toString(), key, value.toString());
    }
}

void PluginSettingsTab::setBooleanValue(bool value) {
    if (const QObject *obj = sender()) {
        m_settings[obj->property("key").toString()] = value;
    }
}

void PluginSettingsTab::setIntegerValue(int value) {
    if (const QObject *obj = sender()) {
        m_settings[obj->property("key").toString()] = value;
    }
}

void PluginSettingsTab::setListValue(int value) {
    if (const QComboBox *combobox = qobject_cast<QComboBox*>(sender())) {
        m_settings[combobox->property("key").toString()] = combobox->itemData(value);
    }
}

void PluginSettingsTab::setTextValue(const QString &value) {
    if (const QObject *obj = sender()) {
        m_settings[obj->property("key").toString()] = value;
    }
}
