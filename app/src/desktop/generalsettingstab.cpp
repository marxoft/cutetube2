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

#include "generalsettingstab.h"
#include "settings.h"
#include <QCheckBox>
#include <QFormLayout>
#include <QLineEdit>

GeneralSettingsTab::GeneralSettingsTab(QWidget *parent) :
    SettingsTab(parent),
    m_commandEdit(new QLineEdit(this)),
    m_clipboardCheckBox(new QCheckBox(tr("Monitor &clipboard for URLs"), this)),
    m_layout(new QFormLayout(this))
{
    setWindowTitle(tr("General"));

    m_layout->addRow(tr("Video &player command (%u for uri):"), m_commandEdit);
    m_layout->addRow(m_clipboardCheckBox);

    restore();
}

void GeneralSettingsTab::restore() {
    m_commandEdit->setText(Settings::videoPlayerCommand());
    m_clipboardCheckBox->setChecked(Settings::clipboardMonitorEnabled());
}

void GeneralSettingsTab::save() {
    Settings::setVideoPlayerCommand(m_commandEdit->text());
    Settings::setClipboardMonitorEnabled(m_clipboardCheckBox->isChecked());
}
