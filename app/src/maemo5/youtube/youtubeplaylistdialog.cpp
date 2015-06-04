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

#include "youtubeplaylistdialog.h"
#include "definitions.h"
#include "imagecache.h"
#include "listview.h"
#include "playlistdelegate.h"
#include "youtubevideo.h"
#include <QTabBar>
#include <QStackedWidget>
#include <QScrollArea>
#include <QLineEdit>
#include <QTextEdit>
#include <QCheckBox>
#include <QDialogButtonBox>
#include <QLabel>
#include <QPushButton>
#include <QGridLayout>
#include <QMessageBox>

YouTubePlaylistDialog::YouTubePlaylistDialog(YouTubeVideo *video, QWidget *parent) :
    Dialog(parent),
    m_video(video),
    m_playlist(0),
    m_model(new YouTubePlaylistModel(this)),
    m_cache(new ImageCache),
    m_view(new ListView(this)),
    m_tabBar(new QTabBar(this)),
    m_stack(new QStackedWidget(this)),
    m_scrollArea(0),
    m_titleEdit(0),
    m_descriptionEdit(0),
    m_privateCheckBox(0),
    m_label(new QLabel(QString("<p align='center'; style='font-size: 40px; color: %1'>%2</p>")
                              .arg(palette().color(QPalette::Mid).name()).arg(tr("No playlists found")), this)),
    m_buttonBox(new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, Qt::Vertical, this)),
    m_layout(new QGridLayout(this))
{
    setWindowTitle(tr("Add to playlist"));
    setMinimumHeight(340);
    
    m_view->setModel(m_model);
    m_view->setItemDelegate(new PlaylistDelegate(m_cache, YouTubePlaylistModel::DateRole,
                                                 YouTubePlaylistModel::ThumbnailUrlRole, YouTubePlaylistModel::TitleRole,
                                                 YouTubePlaylistModel::UsernameRole,
                                                 YouTubePlaylistModel::VideoCountRole, m_view));
    
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
    connect(m_model, SIGNAL(statusChanged(QYouTube::ResourcesRequest::Status)),
            this, SLOT(onModelStatusChanged(QYouTube::ResourcesRequest::Status)));
    connect(m_view, SIGNAL(activated(QModelIndex)), this, SLOT(addToPlaylist(QModelIndex)));
    connect(m_tabBar, SIGNAL(currentChanged(int)), this, SLOT(onTabIndexChanged(int)));
    connect(m_buttonBox, SIGNAL(rejected()), this, SLOT(reject()));
    connect(m_buttonBox, SIGNAL(accepted()), this, SLOT(createNewPlaylist()));         
}

YouTubePlaylistDialog::~YouTubePlaylistDialog() {
    delete m_cache;
    m_cache = 0;
}

void YouTubePlaylistDialog::showEvent(QShowEvent *e) {
    Dialog::showEvent(e);
    getPlaylists();
}

void YouTubePlaylistDialog::getPlaylists() {
    QVariantMap filters;
    filters["mine"] = true;
    
    QVariantMap params;
    params["maxResults"] = MAX_RESULTS;
    
    m_model->list("/playlists", QStringList() << "snippet" << "contentDetails", filters, params);
}

void YouTubePlaylistDialog::showNewPlaylist() {
    if (!m_scrollArea) {
        m_scrollArea = new QScrollArea(this);
        QWidget *scrollWidget = new QWidget(m_scrollArea);
        m_titleEdit = new QLineEdit(scrollWidget);
        m_descriptionEdit = new QTextEdit(scrollWidget);
        m_privateCheckBox = new QCheckBox(tr("Private"), scrollWidget);
        
        QVBoxLayout *vbox = new QVBoxLayout(scrollWidget);
        vbox->addWidget(new QLabel(tr("Title"), scrollWidget));
        vbox->addWidget(m_titleEdit);
        vbox->addWidget(new QLabel(tr("Description"), scrollWidget));
        vbox->addWidget(m_descriptionEdit);
        vbox->addWidget(m_privateCheckBox);
        vbox->setContentsMargins(0, 0, 0, 0);
        
        m_scrollArea->setWidget(scrollWidget);
        m_scrollArea->setWidgetResizable(true);
        
        m_stack->addWidget(m_scrollArea);
        
        connect(m_titleEdit, SIGNAL(textEdited(QString)), this, SLOT(onTitleChanged(QString)));
    }
    
    m_stack->setCurrentWidget(m_scrollArea);
}

void YouTubePlaylistDialog::showPlaylists() {
    if ((m_model->rowCount() == 0) && (m_model->status() != QYouTube::ResourcesRequest::Loading)) {
        m_stack->setCurrentWidget(m_label);
    }
    else {
        m_stack->setCurrentWidget(m_view);
    }
}

void YouTubePlaylistDialog::addToPlaylist(const QModelIndex &index) {
    if (isBusy()) {
        return;
    }
    
    m_playlist = m_model->get(index.row());
    
    if (m_playlist) {
        connect(m_playlist, SIGNAL(statusChanged(QYouTube::ResourcesRequest::Status)),
                this, SLOT(onPlaylistStatusChanged(QYouTube::ResourcesRequest::Status)));
        m_playlist->addVideo(m_video);
    }
}

void YouTubePlaylistDialog::createNewPlaylist() {
    if (isBusy()) {
        return;
    }
    
    QVariantMap playlist;
    QVariantMap snippet;
    QVariantMap status;
    snippet["title"] = m_titleEdit->text();
    
    QString description = m_descriptionEdit->toPlainText();
    
    if (!description.isEmpty()) {
        snippet["description"] = description;
    }
    
    status["privacyStatus"] = m_privateCheckBox->isChecked() ? "private" : "public";
    playlist["snippet"] = snippet;
    playlist["status"] = status;
        
    m_playlist = new YouTubePlaylist(playlist, this);
    connect(m_playlist, SIGNAL(statusChanged(QYouTube::ResourcesRequest::Status)),
                this, SLOT(onPlaylistStatusChanged(QYouTube::ResourcesRequest::Status)));
    
    m_playlist->addVideo(m_video);
}

void YouTubePlaylistDialog::onImageReady() {
    m_view->viewport()->update(m_view->viewport()->rect());
}

void YouTubePlaylistDialog::onModelStatusChanged(QYouTube::ResourcesRequest::Status status) {
    switch (status) {
    case QYouTube::ResourcesRequest::Loading:
        showProgressIndicator();
        return;
    case QYouTube::ResourcesRequest::Failed:
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

void YouTubePlaylistDialog::onPlaylistStatusChanged(QYouTube::ResourcesRequest::Status status) {
    switch (status) {
    case QYouTube::ResourcesRequest::Loading:
        showProgressIndicator();
        return;
    case QYouTube::ResourcesRequest::Ready:
        accept();
        return;
    case QYouTube::ResourcesRequest::Failed:
        QMessageBox::critical(this, tr("Error"), m_playlist->errorString());
        break;
    default:
        break;
    }
    
    hideProgressIndicator();
    disconnect(m_playlist, SIGNAL(statusChanged(QYouTube::ResourcesRequest::Status)),
                this, SLOT(onPlaylistStatusChanged(QYouTube::ResourcesRequest::Status)));
}

void YouTubePlaylistDialog::onTabIndexChanged(int index) {
    if (index == 1) {
        showNewPlaylist();
    }
    else {
        showPlaylists();
    }
}

void YouTubePlaylistDialog::onTitleChanged(const QString &text) {
    m_buttonBox->button(QDialogButtonBox::Ok)->setEnabled(!text.isEmpty());
}
