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

#include "youtubesearchdialog.h"
#include "youtubesearchordermodel.h"
#include "youtubesearchtypemodel.h"
#include "mainwindow.h"
#include "valueselector.h"
#include <QLineEdit>
#include <QDialogButtonBox>
#include <QPushButton>
#include <QGridLayout>

YouTubeSearchDialog::YouTubeSearchDialog(QWidget *parent) :
    Dialog(parent),
    m_typeModel(new YouTubeSearchTypeModel(this)),
    m_orderModel(new YouTubeSearchOrderModel(this)),
    m_typeSelector(new ValueSelector(tr("Search for"), this)),
    m_orderSelector(new ValueSelector(tr("Order by"), this)),
    m_searchEdit(new QLineEdit(this)),
    m_buttonBox(new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, Qt::Vertical, this)),
    m_layout(new QGridLayout(this))
{
    setWindowTitle(tr("Search"));
    
    m_typeSelector->setModel(m_typeModel);
    m_orderSelector->setModel(m_orderModel);
    
    m_searchEdit->setPlaceholderText(tr("Search"));
    
    m_buttonBox->button(QDialogButtonBox::Ok)->setEnabled(false);
    
    m_layout->addWidget(m_searchEdit, 0, 0);
    m_layout->addWidget(m_typeSelector, 1, 0);
    m_layout->addWidget(m_orderSelector, 2, 0);
    m_layout->addWidget(m_buttonBox, 0, 1, 3, 1, Qt::AlignBottom);
    
    connect(m_searchEdit, SIGNAL(textChanged(QString)), this, SLOT(onSearchTextChanged(QString)));
    connect(m_buttonBox, SIGNAL(accepted()), this, SLOT(search()));
    connect(m_buttonBox, SIGNAL(rejected()), this, SLOT(reject()));
}

void YouTubeSearchDialog::search() {
    if (!MainWindow::instance()->showResource(m_searchEdit->text())) {
        MainWindow::instance()->search(Resources::YOUTUBE, m_searchEdit->text(),
                                       m_typeSelector->currentValue().toString(),
                                       m_orderSelector->currentValue().toString());
    }
    
    accept();
}

void YouTubeSearchDialog::onSearchTextChanged(const QString &text) {
    m_buttonBox->button(QDialogButtonBox::Ok)->setEnabled(!text.isEmpty());
}
