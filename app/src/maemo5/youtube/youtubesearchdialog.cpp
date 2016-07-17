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

#include "youtubesearchdialog.h"
#include "resources.h"
#include "searchhistorydialog.h"
#include "settings.h"
#include "valueselector.h"
#include "youtubesearchtypemodel.h"
#include <QLineEdit>
#include <QDialogButtonBox>
#include <QPushButton>
#include <QGridLayout>

YouTubeSearchDialog::YouTubeSearchDialog(QWidget *parent) :
    Dialog(parent),
    m_typeModel(new YouTubeSearchTypeModel(this)),
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
                                    Settings::defaultSearchType(Resources::YOUTUBE))));
    
    m_searchEdit->setPlaceholderText(tr("Search"));
    m_searchEdit->setFocus(Qt::OtherFocusReason);
    
    m_searchButton->setDefault(true);
    m_searchButton->setEnabled(false);
    
    m_buttonBox->addButton(m_historyButton, QDialogButtonBox::ActionRole);
    m_buttonBox->addButton(m_searchButton, QDialogButtonBox::ActionRole);
    
    m_layout->addWidget(m_searchEdit, 0, 0);
    m_layout->addWidget(m_typeSelector, 1, 0);
    m_layout->addWidget(m_buttonBox, 0, 1, 2, 1, Qt::AlignBottom);
    
    connect(m_searchEdit, SIGNAL(textChanged(QString)), this, SLOT(onQueryChanged(QString)));
    connect(m_buttonBox, SIGNAL(rejected()), this, SLOT(reject()));
    connect(m_historyButton, SIGNAL(clicked()), this, SLOT(showHistoryDialog()));
    connect(m_searchButton, SIGNAL(clicked()), this, SLOT(accept()));
}

QString YouTubeSearchDialog::query() const {
    return m_searchEdit->text();
}

void YouTubeSearchDialog::setQuery(const QString &query) {
    m_searchEdit->setText(query);
}

QString YouTubeSearchDialog::order() const {
    return m_typeSelector->currentValue().toMap().value("order").toString();
}

QString YouTubeSearchDialog::type() const {
    return m_typeSelector->currentValue().toMap().value("type").toString();
}

void YouTubeSearchDialog::accept() {
    Settings::addSearch(m_searchEdit->text());
    Settings::setDefaultSearchType(Resources::YOUTUBE, m_typeSelector->valueText());
    Dialog::accept();
}

void YouTubeSearchDialog::showHistoryDialog() {
    SearchHistoryDialog dialog(this);
    
    if (dialog.exec() == QDialog::Accepted) {
        setQuery(dialog.query());
    }
}

void YouTubeSearchDialog::onQueryChanged(const QString &query) {
    m_searchButton->setEnabled(!query.isEmpty());
}
