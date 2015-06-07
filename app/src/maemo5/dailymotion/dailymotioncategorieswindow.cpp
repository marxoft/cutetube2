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

#include "dailymotioncategorieswindow.h"
#include "dailymotionvideoswindow.h"
#include "definitions.h"
#include "listview.h"
#include "settings.h"
#include <QLabel>
#include <QMessageBox>
#include <QVBoxLayout>
#include <QMenuBar>
#include <QAction>

DailymotionCategoriesWindow::DailymotionCategoriesWindow(StackedWindow *parent) :
    StackedWindow(parent),
    m_model(new DailymotionCategoryModel(this)),
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
    
    connect(m_model, SIGNAL(statusChanged(QDailymotion::ResourcesRequest::Status)), this,
            SLOT(onModelStatusChanged(QDailymotion::ResourcesRequest::Status)));
    connect(m_view, SIGNAL(activated(QModelIndex)), this, SLOT(showCategory(QModelIndex)));
    connect(m_reloadAction, SIGNAL(triggered()), m_model, SLOT(reload()));
}

void DailymotionCategoriesWindow::list(const QString &resourcePath, const QVariantMap &filters) {
    m_model->list(resourcePath, filters);
}

void DailymotionCategoriesWindow::showCategory(const QModelIndex &index) {
    QVariantMap filters;
    filters["family_filter"] = Settings::instance()->safeSearchEnabled();
    filters["localization"] = Settings::instance()->locale();
    filters["limit"] = MAX_RESULTS;
    
    DailymotionVideosWindow *window = new DailymotionVideosWindow(this);
    window->setWindowTitle(index.data(DailymotionCategoryModel::NameRole).toString());
    window->list(QString("/channel/%1/videos").arg(index.data(DailymotionCategoryModel::ValueRole).toString()), filters);
    window->show();
}

void DailymotionCategoriesWindow::onModelStatusChanged(QDailymotion::ResourcesRequest::Status status) {
    switch (status) {
    case QDailymotion::ResourcesRequest::Loading:
        showProgressIndicator();
        m_label->hide();
        m_view->show();
        m_reloadAction->setEnabled(false);
        return;
    case QDailymotion::ResourcesRequest::Failed:
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
