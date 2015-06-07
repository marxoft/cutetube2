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

#include "vimeocategorieswindow.h"
#include "definitions.h"
#include "listview.h"
#include "vimeovideoswindow.h"
#include <QLabel>
#include <QMessageBox>
#include <QVBoxLayout>
#include <QMenuBar>
#include <QAction>

VimeoCategoriesWindow::VimeoCategoriesWindow(StackedWindow *parent) :
    StackedWindow(parent),
    m_model(new VimeoCategoryModel(this)),
    m_view(new ListView(this)),
    m_reloadAction(new QAction(tr("Reload"), this)),
    m_label(new QLabel(QString("<p align='center'; style='font-size: 40px; color: %1'>%2</p>")
                              .arg(palette().color(QPalette::Mid).name()).arg(tr("No categories found")), this))
{
    setWindowTitle(tr("Categories"));
    setCentralWidget(new QWidget);
    
    m_view->setModel(m_model);

    m_reloadAction->setEnabled(false);
    
    m_label->hide();
    
    m_layout = new QVBoxLayout(centralWidget());
    m_layout->addWidget(m_view);
    m_layout->addWidget(m_label);
    m_layout->setContentsMargins(0, 0, 0, 0);

    menuBar()->addAction(m_reloadAction);
    
    connect(m_model, SIGNAL(statusChanged(QVimeo::ResourcesRequest::Status)), this,
            SLOT(onModelStatusChanged(QVimeo::ResourcesRequest::Status)));
    connect(m_view, SIGNAL(activated(QModelIndex)), this, SLOT(showCategory(QModelIndex)));
    connect(m_reloadAction, SIGNAL(triggered()), m_model, SLOT(reload()));
}

void VimeoCategoriesWindow::list(const QString &resourcePath, const QVariantMap &filters) {
    m_model->list(resourcePath, filters);
}

void VimeoCategoriesWindow::showCategory(const QModelIndex &index) {
    QVariantMap filters;
    filters["per_page"] = MAX_RESULTS;
    
    VimeoVideosWindow *window = new VimeoVideosWindow(this);
    window->setWindowTitle(index.data(VimeoCategoryModel::NameRole).toString());
    window->list(QString("/categories/%1/videos").arg(index.data(VimeoCategoryModel::ValueRole).toString()), filters);
    window->show();
}

void VimeoCategoriesWindow::onModelStatusChanged(QVimeo::ResourcesRequest::Status status) {
    switch (status) {
    case QVimeo::ResourcesRequest::Loading:
        showProgressIndicator();
        m_label->hide();
        m_view->show();
        m_reloadAction->setEnabled(false);
        return;
    case QVimeo::ResourcesRequest::Failed:
        QMessageBox::critical(this, tr("Error"), m_model->errorString());
        break;
    default:
        break;
    }
    
    hideProgressIndicator();
    m_reloadAction->setEnabled(true);
    
    if (m_model->rowCount() == 0) {
        m_view->hide();
        m_label->show();
    }
}
