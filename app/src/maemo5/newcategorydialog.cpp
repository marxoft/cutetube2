/*
 * Copyright (C) 2014 Stuart Howarth <showarth@marxoft.co.uk>
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

#include "newcategorydialog.h"
#include "settings.h"
#include <QLineEdit>
#include <QMaemo5ValueButton>
#include <QGridLayout>
#include <QDialogButtonBox>
#include <QFileDialog>

NewCategoryDialog::NewCategoryDialog(QWidget *parent) :
    Dialog(parent),
    m_nameEdit(new QLineEdit(this)),
    m_pathSelector(new QMaemo5ValueButton(tr("Download path"), this)),
    m_doneButton(new QPushButton(tr("Done"), this))
{
    setWindowTitle(tr("New category"));

    QDialogButtonBox *buttonBox = new QDialogButtonBox(Qt::Vertical, this);
    buttonBox->addButton(m_doneButton, QDialogButtonBox::AcceptRole);

    QGridLayout *grid = new QGridLayout(this);
    grid->addWidget(m_nameEdit, 0, 0);
    grid->addWidget(m_pathSelector, 1, 0);
    grid->addWidget(buttonBox, 1, 1);

    m_nameEdit->setPlaceholderText(tr("Name"));
    m_nameEdit->setFocus(Qt::OtherFocusReason);
    m_pathSelector->setValueText(tr("None chosen"));
    m_doneButton->setEnabled(false);

    connect(m_nameEdit, SIGNAL(textChanged(QString)), this, SLOT(onNameTextChanged(QString)));
    connect(m_pathSelector, SIGNAL(clicked()), this, SLOT(showFileDialog()));
    connect(buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
}

QString NewCategoryDialog::name() const {
    return m_nameEdit->text();
}

void NewCategoryDialog::setName(const QString &name) {
    m_nameEdit->setText(name);
}

QString NewCategoryDialog::path() const {
    return m_path;
}

void NewCategoryDialog::setPath(const QString &path) {
    m_path = path;
    m_pathSelector->setValueText(path);
}

void NewCategoryDialog::showFileDialog() {
    const QString path = QFileDialog::getExistingDirectory(this, tr("Download path"),
                                                           m_path.isEmpty() ? Settings::downloadPath() : m_path);

    if (!path.isEmpty()) {
        m_path = path;
        m_pathSelector->setValueText(path);
        m_doneButton->setEnabled(!m_nameEdit->text().isEmpty());
    }
    else {
        m_doneButton->setEnabled((!m_path.isEmpty()) && (!m_nameEdit->text().isEmpty()));
    }
}

void NewCategoryDialog::onNameTextChanged(const QString &text) {
    m_doneButton->setEnabled((!text.isEmpty()) && (!m_path.isEmpty()));
}

