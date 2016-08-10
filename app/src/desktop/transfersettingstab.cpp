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

#include "transfersettingstab.h"
#include "definitions.h"
#include "settings.h"
#include <QCheckBox>
#include <QFileDialog>
#include <QFormLayout>
#include <QLineEdit>
#include <QPushButton>
#include <QSpinBox>

TransferSettingsTab::TransferSettingsTab(QWidget *parent) :
    SettingsTab(parent),
    m_pathEdit(new QLineEdit(this)),
    m_commandEdit(new QLineEdit(this)),
    m_pathButton(new QPushButton(QIcon::fromTheme("document-open"), tr("&Browse"), this)),
    m_concurrentSpinBox(new QSpinBox(this)),
    m_commandCheckBox(new QCheckBox(tr("&Enable custom transfer command"), this)),
    m_automaticCheckBox(new QCheckBox(tr("Start transfers &automatically"), this)),
    m_layout(new QFormLayout(this))
{
    setWindowTitle(tr("Transfers"));

    m_concurrentSpinBox->setRange(1, MAX_CONCURRENT_TRANSFERS);

    m_layout->addRow(tr("Download &path:"), m_pathEdit);
    m_layout->addWidget(m_pathButton);
    m_layout->addRow(tr("&Maximum concurrent transfers:"), m_concurrentSpinBox);
    m_layout->addRow(tr("&Custom transfer command (%f for filename):"), m_commandEdit);
    m_layout->addRow(m_commandCheckBox);
    m_layout->addRow(m_automaticCheckBox);

    connect(m_pathButton, SIGNAL(clicked()), this, SLOT(showFileDialog()));

    restore();
}

void TransferSettingsTab::restore() {
    m_pathEdit->setText(Settings::downloadPath());
    m_concurrentSpinBox->setValue(Settings::maximumConcurrentTransfers());
    m_commandEdit->setText(Settings::customTransferCommand());
    m_commandCheckBox->setChecked(Settings::customTransferCommandEnabled());
    m_automaticCheckBox->setChecked(Settings::startTransfersAutomatically());
}

void TransferSettingsTab::save() {
    Settings::setDownloadPath(m_pathEdit->text());
    Settings::setMaximumConcurrentTransfers(m_concurrentSpinBox->value());
    Settings::setCustomTransferCommand(m_commandEdit->text());
    Settings::setCustomTransferCommandEnabled(m_commandCheckBox->isChecked());
    Settings::setStartTransfersAutomatically(m_automaticCheckBox->isChecked());
}

void TransferSettingsTab::showFileDialog() {
    const QString path = QFileDialog::getExistingDirectory(this, tr("Download path"), Settings::downloadPath());

    if (!path.isEmpty()) {
        m_pathEdit->setText(path);
    }
}
