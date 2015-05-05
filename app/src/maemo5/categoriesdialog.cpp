/*
 * Copyright (C) 2014 Stuart Howarth <showarth@marxoft.co.uk>
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms and conditions of the GNU Lesser General Public License,
 * version 3, as published by the Free Software Foundation.
 *
 * This program is distributed in the hope it will be useful, but WITHOUT ANY
 * WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for
 * more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program; if not, write to the Free Software Foundation,
 * Inc., 51 Franklin St - Fifth Floor, Boston, MA 02110-1301 USA.
 */

#include "categoriesdialog.h"
#include "categorymodel.h"
#include "newcategorydialog.h"
#include <QTreeView>
#include <QAction>
#include <QDialogButtonBox>
#include <QPushButton>
#include <QHBoxLayout>

CategoriesDialog::CategoriesDialog(QWidget *parent) :
    Dialog(parent),
    m_model(new CategoryModel(this)),
    m_view(new QTreeView(this))
{
    setWindowTitle(tr("Categories"));
    setMinimumHeight(340);

    QDialogButtonBox *buttonBox = new QDialogButtonBox(Qt::Vertical, this);
    QPushButton *addButton = buttonBox->addButton(tr("New"), QDialogButtonBox::ActionRole);
    QHBoxLayout *hbox = new QHBoxLayout(this);
    hbox->addWidget(m_view);
    hbox->addWidget(buttonBox);
    
    QAction *removeAction = new QAction(tr("Remove"), m_view);
    m_view->addAction(removeAction);
    m_view->setModel(m_model);
    m_view->setRootIsDecorated(false);
    m_view->setSelectionBehavior(QTreeView::SelectRows);
    m_view->setContextMenuPolicy(Qt::ActionsContextMenu);
    m_view->setEditTriggers(QTreeView::NoEditTriggers);
    m_view->setExpandsOnDoubleClick(false);
    m_view->setItemsExpandable(false);
    m_view->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_view->setColumnWidth(0, 200);

    connect(m_view, SIGNAL(activated(QModelIndex)), this, SLOT(editCategory(QModelIndex)));
    connect(removeAction, SIGNAL(triggered()), this, SLOT(removeCategory()));
    connect(addButton, SIGNAL(clicked()), this, SLOT(showNewCategoryDialog()));
}

void CategoriesDialog::editCategory(const QModelIndex &index) {
    NewCategoryDialog *dialog = new NewCategoryDialog(this);
    dialog->setWindowTitle(tr("Edit category"));
    dialog->setName(index.data(CategoryModel::NameRole).toString());
    dialog->setPath(index.data(CategoryModel::PathRole).toString());
    dialog->open();
}

void CategoriesDialog::removeCategory() {
    if (m_view->currentIndex().isValid()) {
        m_model->removeCategory(m_view->currentIndex().row());
    }
}

void CategoriesDialog::showNewCategoryDialog() {
    NewCategoryDialog *dialog = new NewCategoryDialog(this);
    dialog->open();
}
