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

#include "vimeosearchdialog.h"
#include "resources.h"
#include "settings.h"
#include "vimeosearchtypemodel.h"
#include <QComboBox>
#include <QDialogButtonBox>
#include <QFormLayout>
#include <QLineEdit>
#include <QPushButton>

VimeoSearchDialog::VimeoSearchDialog(QWidget *parent) :
    QDialog(parent),
    m_typeModel(new VimeoSearchTypeModel(this)),
    m_queryEdit(new QLineEdit(this)),
    m_typeSelector(new QComboBox(this)),
    m_buttonBox(new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, Qt::Horizontal, this)),
    m_layout(new QFormLayout(this))
{
    setWindowTitle(tr("Search"));

    m_typeSelector->setModel(m_typeModel);
    m_typeSelector->setCurrentIndex(qMax(0,
                                    m_typeModel->match("name", Settings::defaultSearchType(Resources::VIMEO))));

    m_buttonBox->button(QDialogButtonBox::Ok)->setEnabled(false);

    m_layout->addRow(tr("&Query:"), m_queryEdit);
    m_layout->addRow(tr("&Type:"), m_typeSelector);
    m_layout->addRow(m_buttonBox);

    connect(m_queryEdit, SIGNAL(textChanged(QString)), this, SLOT(onQueryChanged(QString)));
    connect(m_buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
    connect(m_buttonBox, SIGNAL(rejected()), this, SLOT(reject()));
}

QString VimeoSearchDialog::query() const {
    return m_queryEdit->text();
}

void VimeoSearchDialog::setQuery(const QString &query) {
    m_queryEdit->setText(query);
}

QString VimeoSearchDialog::order() const {
    return m_typeSelector->itemData(m_typeSelector->currentIndex()).toMap().value("order").toString();
}

QString VimeoSearchDialog::type() const {
    return m_typeSelector->itemData(m_typeSelector->currentIndex()).toMap().value("type").toString();
}

void VimeoSearchDialog::accept() {
    Settings::setDefaultSearchType(Resources::VIMEO, m_typeSelector->currentText());
    QDialog::accept();
}

void VimeoSearchDialog::onQueryChanged(const QString &query) {
    m_buttonBox->button(QDialogButtonBox::Ok)->setEnabled(!query.isEmpty());
}
