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

#include "categorysettingstab.h"
#include "categorymodel.h"
#include <QFileDialog>
#include <QFormLayout>
#include <QHeaderView>
#include <QLineEdit>
#include <QMenu>
#include <QPushButton>
#include <QTreeView>

CategorySettingsTab::CategorySettingsTab(QWidget *parent) :
    SettingsTab(parent),
    m_model(new CategoryModel(this)),
    m_view(new QTreeView(this)),
    m_nameEdit(new QLineEdit(this)),
    m_pathEdit(new QLineEdit(this)),
    m_pathButton(new QPushButton(QIcon::fromTheme("document-open"), tr("&Browse"), this)),
    m_saveButton(new QPushButton(QIcon::fromTheme("document-save"), tr("&Save"), this)),
    m_layout(new QFormLayout(this))
{
    setWindowTitle(tr("Categories"));

    m_view->setModel(m_model);
    m_view->setAlternatingRowColors(true);
    m_view->setSelectionBehavior(QTreeView::SelectRows);
    m_view->setContextMenuPolicy(Qt::CustomContextMenu);
    m_view->setEditTriggers(QTreeView::NoEditTriggers);
    m_view->setItemsExpandable(false);
    m_view->setUniformRowHeights(true);
    m_view->setAllColumnsShowFocus(true);
    m_view->setRootIsDecorated(false);
    m_view->header()->setStretchLastSection(true);

    m_saveButton->setEnabled(false);

    m_layout->addRow(m_view);
    m_layout->addRow(tr("&Name:"), m_nameEdit);
    m_layout->addRow(tr("&Path:"), m_pathEdit);
    m_layout->addWidget(m_pathButton);
    m_layout->addWidget(m_saveButton);

    connect(m_view, SIGNAL(clicked(QModelIndex)), this, SLOT(setCurrentCategory(QModelIndex)));
    connect(m_view, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(showContextMenu(QPoint)));
    connect(m_nameEdit, SIGNAL(textChanged(QString)), this, SLOT(onNameChanged(QString)));
    connect(m_nameEdit, SIGNAL(returnPressed()), m_saveButton, SLOT(animateClick()));
    connect(m_pathEdit, SIGNAL(textChanged(QString)), this, SLOT(onPathChanged(QString)));
    connect(m_pathEdit, SIGNAL(returnPressed()), m_saveButton, SLOT(animateClick()));
    connect(m_pathButton, SIGNAL(clicked()), this, SLOT(showFileDialog()));
    connect(m_saveButton, SIGNAL(clicked()), this, SLOT(addCategory()));
}

void CategorySettingsTab::addCategory() {
    m_model->addCategory(m_nameEdit->text(), m_pathEdit->text());
    m_nameEdit->clear();
    m_pathEdit->clear();
}

void CategorySettingsTab::setCurrentCategory(const QModelIndex &index) {
    m_nameEdit->setText(index.data(CategoryModel::NameRole).toString());
    m_pathEdit->setText(index.data(CategoryModel::PathRole).toString());
}

void CategorySettingsTab::showContextMenu(const QPoint &pos) {
    if (!m_view->currentIndex().isValid()) {
        return;
    }

    QMenu menu(this);
    menu.addAction(QIcon::fromTheme("edit-delete"), tr("&Remove"));

    if (menu.exec(m_view->mapToGlobal(pos))) {
        m_model->removeCategory(m_view->currentIndex().row());
    }
}

void CategorySettingsTab::showFileDialog() {
    const QString path = QFileDialog::getExistingDirectory(this, tr("Path"), m_pathEdit->text());

    if (!path.isEmpty()) {
        m_pathEdit->setText(path);
    }
}

void CategorySettingsTab::onNameChanged(const QString &name) {
    m_saveButton->setEnabled((!name.isEmpty()) && (!m_pathEdit->text().isEmpty()));
}

void CategorySettingsTab::onPathChanged(const QString &path) {
    m_saveButton->setEnabled((!path.isEmpty()) && (!m_nameEdit->text().isEmpty()));
}
