/*
 * Copyright (C) 2015 Stuart Howarth <showarth@marxoft.co.uk>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License version 3 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "vimeosearchdialog.h"
#include "vimeosearchtypemodel.h"
#include "mainwindow.h"
#include "resources.h"
#include "searchhistorydialog.h"
#include "settings.h"
#include "valueselector.h"
#include <QLineEdit>
#include <QDialogButtonBox>
#include <QPushButton>
#include <QGridLayout>

VimeoSearchDialog::VimeoSearchDialog(QWidget *parent) :
    Dialog(parent),
    m_typeModel(new VimeoSearchTypeModel(this)),
    m_typeSelector(new ValueSelector(tr("Search for"), this)),
    m_searchEdit(new QLineEdit(this)),
    m_buttonBox(new QDialogButtonBox(QDialogButtonBox::Cancel, Qt::Vertical, this)),
    m_historyButton(new QPushButton(tr("History"), this)),
    m_searchButton(new QPushButton(tr("Done"), this)),
    m_layout(new QGridLayout(this))
{
    setWindowTitle(tr("Search"));
        
    m_typeSelector->setModel(m_typeModel);
    m_typeSelector->setCurrentIndex(qMax(0, m_typeModel->match("name",
                                    Settings::instance()->defaultSearchType(Resources::VIMEO))));
    
    m_searchEdit->setPlaceholderText(tr("Search"));
    m_searchEdit->setFocus(Qt::OtherFocusReason);
    
    m_searchButton->setDefault(true);
    m_searchButton->setEnabled(false);
    
    m_buttonBox->addButton(m_historyButton, QDialogButtonBox::ActionRole);
    m_buttonBox->addButton(m_searchButton, QDialogButtonBox::ActionRole);
    
    m_layout->addWidget(m_searchEdit, 0, 0);
    m_layout->addWidget(m_typeSelector, 1, 0);
    m_layout->addWidget(m_buttonBox, 0, 1, 2, 1, Qt::AlignBottom);
    
    connect(m_typeSelector, SIGNAL(valueChanged(QVariant)), this, SLOT(onSearchTypeChanged()));
    connect(m_searchEdit, SIGNAL(textChanged(QString)), this, SLOT(onSearchTextChanged(QString)));
    connect(m_buttonBox, SIGNAL(rejected()), this, SLOT(reject()));
    connect(m_historyButton, SIGNAL(clicked()), this, SLOT(showHistoryDialog()));
    connect(m_searchButton, SIGNAL(clicked()), this, SLOT(search()));
}

void VimeoSearchDialog::search() {
    if (!MainWindow::instance()->showResource(m_searchEdit->text())) {
        QVariantMap type = m_typeSelector->currentValue().toMap();
        Settings::instance()->addSearch(m_searchEdit->text());
        MainWindow::instance()->search(Resources::VIMEO, m_searchEdit->text(), type.value("type").toString(),
                                       type.value("order").toString());
    }
    
    accept();
}

void VimeoSearchDialog::showHistoryDialog() {
    SearchHistoryDialog *dialog = new SearchHistoryDialog(this);
    dialog->open();
    connect(dialog, SIGNAL(searchChosen(QString)), m_searchEdit, SLOT(setText(QString)));
}

void VimeoSearchDialog::onSearchTextChanged(const QString &text) {
    m_searchButton->setEnabled(!text.isEmpty());
}

void VimeoSearchDialog::onSearchTypeChanged() {
    Settings::instance()->setDefaultSearchType(Resources::VIMEO, m_typeSelector->valueText());
}
