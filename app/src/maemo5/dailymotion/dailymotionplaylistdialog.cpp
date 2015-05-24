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

#include "dailymotionplaylistdialog.h"
#include "dailymotion.h"
#include "dailymotionvideo.h"
#include "definitions.h"
#include "imagecache.h"
#include "listview.h"
#include "playlistdelegate.h"
#include <QTabBar>
#include <QStackedWidget>
#include <QLineEdit>
#include <QTextEdit>
#include <QDialogButtonBox>
#include <QLabel>
#include <QPushButton>
#include <QGridLayout>
#include <QMessageBox>

DailymotionPlaylistDialog::DailymotionPlaylistDialog(DailymotionVideo *video, QWidget *parent) :
    Dialog(parent),
    m_video(video),
    m_playlist(0),
    m_model(new DailymotionPlaylistModel(this)),
    m_cache(new ImageCache),
    m_view(new ListView(this)),
    m_tabBar(new QTabBar(this)),
    m_stack(new QStackedWidget(this)),
    m_widget(0),
    m_titleEdit(0),
    m_descriptionEdit(0),
    m_label(new QLabel(QString("<p align='center'; style='font-size: 40px; color: %1'>%2</p>")
                              .arg(palette().color(QPalette::Mid).name()).arg(tr("No playlists found")), this)),
    m_buttonBox(new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, Qt::Vertical, this)),
    m_layout(new QGridLayout(this))
{
    setWindowTitle(tr("Add to playlist"));
    setMinimumHeight(340);
    
    m_view->setModel(m_model);
    m_view->setItemDelegate(new PlaylistDelegate(m_cache, DailymotionPlaylistModel::DateRole,
                                                 DailymotionPlaylistModel::ThumbnailUrlRole,
                                                 DailymotionPlaylistModel::TitleRole,
                                                 DailymotionPlaylistModel::UsernameRole,
                                                 DailymotionPlaylistModel::VideoCountRole, m_view));
    m_tabBar->addTab(tr("Playlists"));
    m_tabBar->addTab(tr("New playlist"));
    m_tabBar->setExpanding(false);
    m_tabBar->setDrawBase(false);
    m_tabBar->setStyleSheet("QTabBar::tab { height: 40px; }");
    
    m_stack->addWidget(m_view);
    m_stack->addWidget(m_label);
    
    m_buttonBox->button(QDialogButtonBox::Ok)->setEnabled(false);
    
    m_layout->addWidget(m_tabBar, 0, 0);
    m_layout->addWidget(m_stack, 1, 0);
    m_layout->addWidget(m_buttonBox, 1, 1, 2, 1);
    m_layout->setRowStretch(1, 1);
        
    connect(m_cache, SIGNAL(imageReady()), this, SLOT(onImageReady()));
    connect(m_model, SIGNAL(statusChanged(QDailymotion::ResourcesRequest::Status)),
            this, SLOT(onModelStatusChanged(QDailymotion::ResourcesRequest::Status)));
    connect(m_view, SIGNAL(activated(QModelIndex)), this, SLOT(addToPlaylist(QModelIndex)));
    connect(m_tabBar, SIGNAL(currentChanged(int)), this, SLOT(onTabIndexChanged(int)));
    connect(m_buttonBox, SIGNAL(rejected()), this, SLOT(reject()));
    connect(m_buttonBox, SIGNAL(accepted()), this, SLOT(createNewPlaylist()));         
}

DailymotionPlaylistDialog::~DailymotionPlaylistDialog() {
    delete m_cache;
    m_cache = 0;
}

void DailymotionPlaylistDialog::showEvent(QShowEvent *e) {
    Dialog::showEvent(e);
    getPlaylists();
}

void DailymotionPlaylistDialog::getPlaylists() {
    QVariantMap filters;
    filters["limit"] = MAX_RESULTS;
    
    m_model->list("/me/playlists", filters);
}

void DailymotionPlaylistDialog::showNewPlaylist() {
    if (!m_widget) {
        m_widget = new QWidget(this);
        m_titleEdit = new QLineEdit(m_widget);
        m_descriptionEdit = new QTextEdit(m_widget);
        
        QVBoxLayout *vbox = new QVBoxLayout(m_widget);
        vbox->addWidget(new QLabel(tr("Title"), m_widget));
        vbox->addWidget(m_titleEdit);
        vbox->addWidget(new QLabel(tr("Description"), m_widget));
        vbox->addWidget(m_descriptionEdit);
        vbox->setContentsMargins(0, 0, 0, 0);
        
        m_stack->addWidget(m_widget);
        
        connect(m_titleEdit, SIGNAL(textEdited(QString)), this, SLOT(onTitleChanged(QString)));
    }
    
    m_stack->setCurrentWidget(m_widget);
}

void DailymotionPlaylistDialog::showPlaylists() {
    if ((m_model->rowCount() == 0) && (m_model->status() != QDailymotion::ResourcesRequest::Loading)) {
        m_stack->setCurrentWidget(m_label);
    }
    else {
        m_stack->setCurrentWidget(m_view);
    }
}

void DailymotionPlaylistDialog::addToPlaylist(const QModelIndex &index) {
    if (isBusy()) {
        return;
    }
    
    m_playlist = m_model->get(index.row());
    
    if (m_playlist) {
        connect(m_playlist, SIGNAL(statusChanged(QDailymotion::ResourcesRequest::Status)),
                this, SLOT(onPlaylistStatusChanged(QDailymotion::ResourcesRequest::Status)));
        m_playlist->addVideo(m_video);
    }
}

void DailymotionPlaylistDialog::createNewPlaylist() {
    if (isBusy()) {
        return;
    }
    
    QVariantMap playlist;
    playlist["name"] = m_titleEdit->text();
    
    QString description = m_descriptionEdit->toPlainText();
    
    if (!description.isEmpty()) {
        playlist["description"] = description;
    }
        
    m_playlist = new DailymotionPlaylist(playlist, this);
    connect(m_playlist, SIGNAL(statusChanged(QDailymotion::ResourcesRequest::Status)),
                this, SLOT(onPlaylistStatusChanged(QDailymotion::ResourcesRequest::Status)));
    
    m_playlist->addVideo(m_video);
}

void DailymotionPlaylistDialog::onImageReady() {
    m_view->viewport()->update(m_view->viewport()->rect());
}

void DailymotionPlaylistDialog::onModelStatusChanged(QDailymotion::ResourcesRequest::Status status) {
    switch (status) {
    case QDailymotion::ResourcesRequest::Loading:
        showProgressIndicator();
        return;
    case QDailymotion::ResourcesRequest::Failed:
        QMessageBox::critical(this, tr("Error"), m_model->errorString());
        break;
    default:
        break;
    }
    
    hideProgressIndicator();
    
    if (m_view->isVisible()) {
        showPlaylists();
    }
}

void DailymotionPlaylistDialog::onPlaylistStatusChanged(QDailymotion::ResourcesRequest::Status status) {
    switch (status) {
    case QDailymotion::ResourcesRequest::Loading:
        showProgressIndicator();
        return;
    case QDailymotion::ResourcesRequest::Ready:
        accept();
        return;
    case QDailymotion::ResourcesRequest::Failed:
        QMessageBox::critical(this, tr("Error"), m_playlist->errorString());
        break;
    default:
        break;
    }
    
    hideProgressIndicator();
    disconnect(m_playlist, SIGNAL(statusChanged(QDailymotion::ResourcesRequest::Status)),
                this, SLOT(onPlaylistStatusChanged(QDailymotion::ResourcesRequest::Status)));
}

void DailymotionPlaylistDialog::onTabIndexChanged(int index) {
    if (index == 1) {
        showNewPlaylist();
    }
    else {
        showPlaylists();
    }
}

void DailymotionPlaylistDialog::onTitleChanged(const QString &text) {
    m_buttonBox->button(QDialogButtonBox::Ok)->setEnabled(!text.isEmpty());
}
