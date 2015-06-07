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

#include "youtubecategorieswindow.h"
#include "definitions.h"
#include "listview.h"
#include "settings.h"
#include "youtubevideoswindow.h"
#include <QLabel>
#include <QMessageBox>
#include <QVBoxLayout>
#include <QMenuBar>
#include <QAction>

YouTubeCategoriesWindow::YouTubeCategoriesWindow(StackedWindow *parent) :
    StackedWindow(parent),
    m_model(new YouTubeCategoryModel(this)),
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
    
    connect(m_model, SIGNAL(statusChanged(QYouTube::ResourcesRequest::Status)), this,
            SLOT(onModelStatusChanged(QYouTube::ResourcesRequest::Status)));
    connect(m_view, SIGNAL(activated(QModelIndex)), this, SLOT(showCategory(QModelIndex)));
    connect(m_reloadAction, SIGNAL(triggered()), m_model, SLOT(reload()));
}

void YouTubeCategoriesWindow::list(const QString &resourcePath, const QStringList &part, const QVariantMap &filters,
                                   const QVariantMap &params) {
    m_model->list(resourcePath, part, filters, params);
}

void YouTubeCategoriesWindow::showCategory(const QModelIndex &index) {
    QVariantMap filters;
    filters["chart"] = "mostPopular";
    
    QVariantMap params;
    params["videoCategoryId"] = index.data(YouTubeCategoryModel::ValueRole);
    params["regionCode"] = Settings::instance()->locale().section('_', -1);
    params["maxResults"] = MAX_RESULTS;
    
    YouTubeVideosWindow *window = new YouTubeVideosWindow(this);
    window->setWindowTitle(index.data(YouTubeCategoryModel::NameRole).toString());
    window->list("/videos", QStringList() << "snippet" << "contentDetails" << "statistics", filters, params);
    window->show();
}

void YouTubeCategoriesWindow::onModelStatusChanged(QYouTube::ResourcesRequest::Status status) {
    switch (status) {
    case QYouTube::ResourcesRequest::Loading:
        showProgressIndicator();
        m_label->hide();
        m_view->show();
        m_reloadAction->setEnabled(false);
        return;
    case QYouTube::ResourcesRequest::Failed:
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
