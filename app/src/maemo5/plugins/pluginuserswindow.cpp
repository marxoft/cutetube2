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

#include "pluginuserswindow.h"
#include "imagecache.h"
#include "listview.h"
#include "pluginuserwindow.h"
#include "userdelegate.h"
#include <QLabel>
#include <QMessageBox>
#include <QVBoxLayout>
#include <QMenuBar>

PluginUsersWindow::PluginUsersWindow(StackedWindow *parent) :
    StackedWindow(parent),
    m_model(new PluginUserModel(this)),
    m_cache(new ImageCache),
    m_view(new ListView(this)),
    m_delegate(new UserDelegate(m_cache, -1, PluginUserModel::ThumbnailUrlRole, PluginUserModel::UsernameRole, m_view)),
    m_reloadAction(new QAction(tr("Reload"), this)),
    m_label(new QLabel(QString("<p align='center'; style='font-size: 40px; color: %1'>%2</p>")
                              .arg(palette().color(QPalette::Mid).name()).arg(tr("No users found")), this))
{
    setWindowTitle(tr("Users"));
    setCentralWidget(new QWidget);
    
    m_view->setModel(m_model);
    m_view->setItemDelegate(m_delegate);

    m_reloadAction->setEnabled(false);
    
    m_label->hide();
    
    m_layout = new QVBoxLayout(centralWidget());
    m_layout->addWidget(m_view);
    m_layout->addWidget(m_label);
    m_layout->setContentsMargins(0, 0, 0, 0);
    
    menuBar()->addAction(m_reloadAction);
    
    connect(m_model, SIGNAL(statusChanged(ResourcesRequest::Status)), this,
            SLOT(onModelStatusChanged(ResourcesRequest::Status)));
    connect(m_cache, SIGNAL(imageReady()), this, SLOT(onImageReady()));
    connect(m_view, SIGNAL(activated(QModelIndex)), this, SLOT(showUser(QModelIndex)));
    connect(m_reloadAction, SIGNAL(triggered()), m_model, SLOT(reload()));
}

PluginUsersWindow::~PluginUsersWindow() {
    delete m_cache;
    m_cache = 0;
}

void PluginUsersWindow::list(const QString &service, const QString &id) {
    m_model->setService(service);
    m_model->list(id);
}

void PluginUsersWindow::search(const QString &service, const QString &query, const QString &order) {
    m_model->setService(service);
    m_model->search(query, order);
}

void PluginUsersWindow::showUser(const QModelIndex &index) {
    if (const PluginUser *user = m_model->get(index.row())) {
        PluginUserWindow *window = new PluginUserWindow(user, this);
        window->show();
    }
}

void PluginUsersWindow::onImageReady() {
    m_view->viewport()->update(m_view->viewport()->rect());
}

void PluginUsersWindow::onModelStatusChanged(ResourcesRequest::Status status) {
    switch (status) {
    case ResourcesRequest::Loading:
        showProgressIndicator();
        m_label->hide();
        m_view->show();
        m_reloadAction->setEnabled(false);
        return;
    case ResourcesRequest::Failed:
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
