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

#include "dailymotionplaylistswindow.h"
#include "dailymotionplaylistwindow.h"
#include "imagecache.h"
#include "listview.h"
#include "playlistdelegate.h"
#include <QLabel>
#include <QMessageBox>
#include <QVBoxLayout>
#include <QMenuBar>

DailymotionPlaylistsWindow::DailymotionPlaylistsWindow(StackedWindow *parent) :
    StackedWindow(parent),
    m_model(new DailymotionPlaylistModel(this)),
    m_cache(new ImageCache),
    m_view(new ListView(this)),
    m_delegate(new PlaylistDelegate(m_cache, DailymotionPlaylistModel::DateRole,
                                    DailymotionPlaylistModel::ThumbnailUrlRole, DailymotionPlaylistModel::TitleRole,
                                    DailymotionPlaylistModel::UsernameRole, DailymotionPlaylistModel::VideoCountRole,
                                    m_view)),
    m_reloadAction(new QAction(tr("Reload"), this)),
    m_label(new QLabel(QString("<p align='center'; style='font-size: 40px; color: %1'>%2</p>")
                              .arg(palette().color(QPalette::Mid).name()).arg(tr("No playlists found")), this))
{
    setWindowTitle(tr("Playlists"));
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
    
    connect(m_model, SIGNAL(statusChanged(QDailymotion::ResourcesRequest::Status)), this,
            SLOT(onModelStatusChanged(QDailymotion::ResourcesRequest::Status)));
    connect(m_cache, SIGNAL(imageReady()), this, SLOT(onImageReady()));
    connect(m_view, SIGNAL(activated(QModelIndex)), this, SLOT(showPlaylist(QModelIndex)));
    connect(m_reloadAction, SIGNAL(triggered()), m_model, SLOT(reload()));
}

DailymotionPlaylistsWindow::~DailymotionPlaylistsWindow() {
    delete m_cache;
    m_cache = 0;
}

void DailymotionPlaylistsWindow::list(const QString &resourcePath, const QVariantMap &filters) {
    m_model->list(resourcePath, filters);
}

void DailymotionPlaylistsWindow::showPlaylist(const QModelIndex &index) {
    if (const DailymotionPlaylist *playlist = m_model->get(index.row())) {
        DailymotionPlaylistWindow *window = new DailymotionPlaylistWindow(playlist, this);
        window->show();
    }
}

void DailymotionPlaylistsWindow::onImageReady() {
    m_view->viewport()->update(m_view->viewport()->rect());
}

void DailymotionPlaylistsWindow::onModelStatusChanged(QDailymotion::ResourcesRequest::Status status) {
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
