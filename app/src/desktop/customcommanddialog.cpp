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

#include "customcommanddialog.h"
#include <QCheckBox>
#include <QDialogButtonBox>
#include <QFormLayout>
#include <QLineEdit>

CustomCommandDialog::CustomCommandDialog(QWidget *parent) :
    QDialog(parent),
    m_commandEdit(new QLineEdit(this)),
    m_overrideCheckBox(new QCheckBox(tr("&Override global command"), this)),
    m_buttonBox(new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, Qt::Horizontal, this)),
    m_layout(new QFormLayout(this))
{
    setWindowTitle(tr("Set custom command"));
    
    m_layout->addRow(tr("&Command:"), m_commandEdit);
    m_layout->addRow(m_overrideCheckBox);
    m_layout->addRow(m_buttonBox);
    
    connect(m_buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
    connect(m_buttonBox, SIGNAL(rejected()), this, SLOT(reject()));
}

QString CustomCommandDialog::command() const {
    return m_commandEdit->text();
}

void CustomCommandDialog::setCommand(const QString &command) {
    m_commandEdit->setText(command);
}

bool CustomCommandDialog::overrideEnabled() const {
    return m_overrideCheckBox->isChecked();
}

void CustomCommandDialog::setOverrideEnabled(bool enabled) {
    m_overrideCheckBox->setChecked(enabled);
}
