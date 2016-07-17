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

#include "searchhistorydialog.h"
#include "filterbox.h"
#include "listview.h"
#include "searchhistorymodel.h"
#include <QDialogButtonBox>
#include <QPushButton>
#include <QAction>
#include <QHBoxLayout>
#include <QKeyEvent>

SearchHistoryDialog::SearchHistoryDialog(QWidget *parent) :
    Dialog(parent),
    m_model(new SearchHistoryModel(this)),
    m_view(new ListView(this)),
    m_filterBox(new FilterBox(this)),
    m_removeAction(new QAction(tr("Remove"), this)),
    m_buttonBox(new QDialogButtonBox(QDialogButtonBox::Cancel, Qt::Vertical, this)),
    m_clearButton(new QPushButton(tr("Clear"), this)),
    m_layout(new QHBoxLayout(this))
{
    setWindowTitle(tr("Search history"));
    setMinimumHeight(360);
    
    m_view->setModel(m_model);
    m_view->addAction(m_removeAction);
    m_view->setContextMenuPolicy(Qt::ActionsContextMenu);
    m_view->setFocusPolicy(Qt::NoFocus);
            
    m_clearButton->setDefault(false);
    m_buttonBox->addButton(m_clearButton, QDialogButtonBox::ActionRole);
    
    QWidget *widget = new QWidget(this);
    QVBoxLayout *vbox = new QVBoxLayout(widget);
    vbox->addWidget(m_view);
    vbox->addWidget(m_filterBox);
    vbox->setStretch(0, 1);
    vbox->setContentsMargins(0, 0, 0, 0);
    
    m_layout->addWidget(widget);
    m_layout->addWidget(m_buttonBox);
    
    m_filterBox->hide();
    
    connect(m_model, SIGNAL(countChanged(int)), this, SLOT(onCountChanged(int)));
    connect(m_view, SIGNAL(activated(QModelIndex)), this, SLOT(accept()));
    connect(m_filterBox, SIGNAL(textChanged(QString)), this, SLOT(onFilterTextChanged(QString)));
    connect(m_removeAction, SIGNAL(triggered()), this, SLOT(removeSearch()));
    connect(m_clearButton, SIGNAL(clicked()), m_model, SLOT(clear()));
    
    onCountChanged(m_model->rowCount());
}

QString SearchHistoryDialog::query() const {
    return m_query;
}

void SearchHistoryDialog::setQuery(const QString &query) {
    m_query = query;
}

void SearchHistoryDialog::accept() {
    setQuery(m_view->currentIndex().data().toString());
    Dialog::accept();
}

void SearchHistoryDialog::keyPressEvent(QKeyEvent *e) {
    if ((m_filterBox->isHidden()) && (e->key() >= Qt::Key_0) && (e->key() <= Qt::Key_Z)) {
        m_filterBox->setText(e->text());
        m_filterBox->setFocus(Qt::OtherFocusReason);
    }
}

void SearchHistoryDialog::removeSearch() {
    m_model->removeSearch(m_view->currentIndex().data().toString());
}

void SearchHistoryDialog::onCountChanged(int count) {
    m_clearButton->setEnabled(count > 0);
}

void SearchHistoryDialog::onFilterTextChanged(const QString &text) {
    m_model->setFilterFixedString(text);
    m_filterBox->setVisible(!text.isEmpty());
}
