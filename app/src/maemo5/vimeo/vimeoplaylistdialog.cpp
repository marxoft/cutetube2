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

#include "vimeoplaylistdialog.h"
#include "definitions.h"
#include "imagecache.h"
#include "listview.h"
#include "vimeo.h"
#include "vimeoplaylistdelegate.h"
#include "vimeovideo.h"
#include <QTabBar>
#include <QStackedWidget>
#include <QScrollArea>
#include <QLineEdit>
#include <QTextEdit>
#include <QDialogButtonBox>
#include <QLabel>
#include <QPushButton>
#include <QGridLayout>
#include <QMessageBox>

VimeoPlaylistDialog::VimeoPlaylistDialog(const VimeoVideo *video, QWidget *parent) :
    Dialog(parent),
    m_video(video),
    m_playlist(0),
    m_model(new VimeoPlaylistModel(this)),
    m_cache(new ImageCache),
    m_view(new ListView(this)),
    m_tabBar(new QTabBar(this)),
    m_stack(new QStackedWidget(this)),
    m_scrollArea(0),
    m_titleEdit(0),
    m_descriptionEdit(0),
    m_passwordEdit(0),
    m_label(new QLabel(QString("<p align='center'; style='font-size: 40px; color: %1'>%2</p>")
                              .arg(palette().color(QPalette::Mid).name()).arg(tr("No albums found")), this)),
    m_buttonBox(new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, Qt::Vertical, this)),
    m_layout(new QGridLayout(this))
{
    setWindowTitle(tr("Add to album"));
    setMinimumHeight(340);
    
    m_view->setModel(m_model);
    m_view->setItemDelegate(new VimeoPlaylistDelegate(m_cache, m_view));
    
    m_tabBar->addTab(tr("Albums"));
    m_tabBar->addTab(tr("New album"));
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
    connect(m_model, SIGNAL(statusChanged(QVimeo::ResourcesRequest::Status)),
            this, SLOT(onModelStatusChanged(QVimeo::ResourcesRequest::Status)));
    connect(m_view, SIGNAL(activated(QModelIndex)), this, SLOT(addToPlaylist(QModelIndex)));
    connect(m_tabBar, SIGNAL(currentChanged(int)), this, SLOT(onTabIndexChanged(int)));
    connect(m_buttonBox, SIGNAL(rejected()), this, SLOT(reject()));
    connect(m_buttonBox, SIGNAL(accepted()), this, SLOT(createNewPlaylist()));         
}

VimeoPlaylistDialog::~VimeoPlaylistDialog() {
    delete m_cache;
    m_cache = 0;
}

void VimeoPlaylistDialog::showEvent(QShowEvent *e) {
    Dialog::showEvent(e);
    getPlaylists();
}

void VimeoPlaylistDialog::getPlaylists() {
    QVariantMap filters;
    filters["per_page"] = MAX_RESULTS;
    
    m_model->list("/me/albums", filters);
}

void VimeoPlaylistDialog::showNewPlaylist() {
    if (!m_scrollArea) {
        m_scrollArea = new QScrollArea(this);
        QWidget *scrollWidget = new QWidget(m_scrollArea);
        m_titleEdit = new QLineEdit(scrollWidget);
        m_descriptionEdit = new QTextEdit(scrollWidget);
        m_passwordEdit = new QLineEdit(scrollWidget);
        m_passwordEdit->setEchoMode(QLineEdit::Password);
        
        QVBoxLayout *vbox = new QVBoxLayout(scrollWidget);
        vbox->addWidget(new QLabel(tr("Title"), scrollWidget));
        vbox->addWidget(m_titleEdit);
        vbox->addWidget(new QLabel(tr("Description"), scrollWidget));
        vbox->addWidget(m_descriptionEdit);
        vbox->addWidget(new QLabel(tr("Password (if private)"), scrollWidget));
        vbox->addWidget(m_passwordEdit);
        vbox->setContentsMargins(0, 0, 0, 0);
        
        m_scrollArea->setWidget(scrollWidget);
        m_scrollArea->setWidgetResizable(true);
        
        m_stack->addWidget(m_scrollArea);
        
        connect(m_titleEdit, SIGNAL(textEdited(QString)), this, SLOT(onTitleChanged(QString)));
    }
    
    m_stack->setCurrentWidget(m_scrollArea);
}

void VimeoPlaylistDialog::showPlaylists() {
    if ((m_model->rowCount() == 0) && (m_model->status() != QVimeo::ResourcesRequest::Loading)) {
        m_stack->setCurrentWidget(m_label);
    }
    else {
        m_stack->setCurrentWidget(m_view);
    }
}

void VimeoPlaylistDialog::addToPlaylist(const QModelIndex &index) {
    if (isBusy()) {
        return;
    }
    
    m_playlist = m_model->get(index.row());
    
    if (m_playlist) {
        connect(m_playlist, SIGNAL(statusChanged(QVimeo::ResourcesRequest::Status)),
                this, SLOT(onPlaylistStatusChanged(QVimeo::ResourcesRequest::Status)));
        m_playlist->addVideo(m_video);
    }
}

void VimeoPlaylistDialog::createNewPlaylist() {
    if (isBusy()) {
        return;
    }
    
    QVariantMap playlist;
    playlist["name"] = m_titleEdit->text();
    
    QString description = m_descriptionEdit->toPlainText();
    
    if (!description.isEmpty()) {
        playlist["description"] = description;
    }
        
    if (!m_passwordEdit->text().isEmpty()) {
        playlist["privacy"] = "password";
        playlist["password"] = m_passwordEdit->text();
    }
        
    m_playlist = new VimeoPlaylist(playlist, this);
    connect(m_playlist, SIGNAL(statusChanged(QVimeo::ResourcesRequest::Status)),
                this, SLOT(onPlaylistStatusChanged(QVimeo::ResourcesRequest::Status)));
    
    m_playlist->addVideo(m_video);
}

void VimeoPlaylistDialog::onImageReady() {
    m_view->viewport()->update(m_view->viewport()->rect());
}

void VimeoPlaylistDialog::onModelStatusChanged(QVimeo::ResourcesRequest::Status status) {
    switch (status) {
    case QVimeo::ResourcesRequest::Loading:
        showProgressIndicator();
        return;
    case QVimeo::ResourcesRequest::Failed:
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

void VimeoPlaylistDialog::onPlaylistStatusChanged(QVimeo::ResourcesRequest::Status status) {
    switch (status) {
    case QVimeo::ResourcesRequest::Loading:
        showProgressIndicator();
        return;
    case QVimeo::ResourcesRequest::Ready:
        accept();
        return;
    case QVimeo::ResourcesRequest::Failed:
        QMessageBox::critical(this, tr("Error"), m_playlist->errorString());
        break;
    default:
        break;
    }
    
    hideProgressIndicator();
    disconnect(m_playlist, SIGNAL(statusChanged(QVimeo::ResourcesRequest::Status)),
                this, SLOT(onPlaylistStatusChanged(QVimeo::ResourcesRequest::Status)));
}

void VimeoPlaylistDialog::onTabIndexChanged(int index) {
    if (index == 1) {
        showNewPlaylist();
    }
    else {
        showPlaylists();
    }
}

void VimeoPlaylistDialog::onTitleChanged(const QString &text) {
    m_buttonBox->button(QDialogButtonBox::Ok)->setEnabled(!text.isEmpty());
}
