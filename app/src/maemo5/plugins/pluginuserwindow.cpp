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

#include "pluginuserwindow.h"
#include "image.h"
#include "listview.h"
#include "navdelegate.h"
#include "pluginplaylistswindow.h"
#include "pluginplaylistwindow.h"
#include "pluginuserswindow.h"
#include "pluginvideoswindow.h"
#include "pluginvideowindow.h"
#include "resources.h"
#include "textbrowser.h"
#include "utils.h"
#include <QScrollArea>
#include <QLabel>
#include <QPushButton>
#include <QGridLayout>
#include <QMessageBox>
#include <QMaemo5InformationBox>
#include <QStringListModel>
#include <QDesktopServices>

PluginUserWindow::PluginUserWindow(const QString &service, const QString &id, StackedWindow *parent) :
    StackedWindow(parent),
    m_user(new PluginUser(this)),
    m_model(new QStringListModel(QStringList() << tr("Videos") << tr("Playlists"), this)),
    m_avatar(new Image(this)),
    m_view(new ListView(this)),
    m_scrollArea(new QScrollArea(this)),
    m_titleLabel(new QLabel(this)),
    m_descriptionLabel(new TextBrowser(this))
{
    loadBaseUi();
    connect(m_user, SIGNAL(statusChanged(ResourcesRequest::Status)),
            this, SLOT(onUserStatusChanged(ResourcesRequest::Status)));
            
    m_user->loadUser(service, id);
}

PluginUserWindow::PluginUserWindow(const PluginUser *user, StackedWindow *parent) :
    StackedWindow(parent),
    m_user(new PluginUser(user, this)),
    m_model(new QStringListModel(QStringList() << tr("Videos") << tr("Playlists"), this)),
    m_avatar(new Image(this)),
    m_view(new ListView(this)),
    m_scrollArea(new QScrollArea(this)),
    m_titleLabel(new QLabel(this)),
    m_descriptionLabel(new TextBrowser(this))
{
    loadBaseUi();
    loadUserUi();
}

void PluginUserWindow::loadBaseUi() {
    setWindowTitle(tr("User"));
    setCentralWidget(new QWidget);
    
    m_view->setModel(m_model);
    m_view->setItemDelegate(new NavDelegate(m_view));
    
    m_avatar->setFixedSize(100, 100);
    
    m_titleLabel->setWordWrap(true);
        
    QWidget *scrollWidget = new QWidget(m_scrollArea);
    QVBoxLayout *vbox = new QVBoxLayout(scrollWidget);
    vbox->addWidget(m_avatar, Qt::AlignLeft);
    vbox->addWidget(m_titleLabel, Qt::AlignLeft | Qt::AlignTop);
    vbox->addWidget(m_descriptionLabel, Qt::AlignTop);
    vbox->setStretch(2, 1);
    vbox->setContentsMargins(0, 0, 0, 0);
    m_scrollArea->setWidget(scrollWidget);
    m_scrollArea->setWidgetResizable(true);
    m_scrollArea->setFixedWidth(410);
    
    m_layout = new QHBoxLayout(centralWidget());
    m_layout->addWidget(m_scrollArea);
    m_layout->addWidget(m_view);
    m_layout->setStretch(1, 1);
    m_layout->setContentsMargins(0, 0, 0, 0);
    
    connect(m_view, SIGNAL(activated(QModelIndex)), this, SLOT(showWindow(QModelIndex)));
    connect(m_descriptionLabel, SIGNAL(anchorClicked(QUrl)), this, SLOT(showResource(QUrl)));
}

void PluginUserWindow::loadUserUi() {
    m_titleLabel->setText(m_user->username());
    m_descriptionLabel->setHtml(Utils::toRichText(m_user->description()));
    m_avatar->setSource(m_user->largeThumbnailUrl());
}

void PluginUserWindow::showPlaylists() {
    const QString playlistsId = m_user->playlistsId();
    
    if (playlistsId.isEmpty()) {
        QMaemo5InformationBox::information(this, tr("This user does not have any playlists"));
        return;
    }
    
    PluginPlaylistsWindow *window = new PluginPlaylistsWindow(this);
    window->setWindowTitle(tr("%1's playlists").arg(m_user->username()));
    window->list(m_user->service(), playlistsId);
    window->show();
}

void PluginUserWindow::showVideos() {
    const QString videosId = m_user->videosId();
    
    if (videosId.isEmpty()) {
        QMaemo5InformationBox::information(this, tr("This user does not have any videos"));
        return;
    }
    
    PluginVideosWindow *window = new PluginVideosWindow(this);
    window->setWindowTitle(tr("%1's videos").arg(m_user->username()));
    window->list(m_user->service(), videosId);
    window->show();
}

void PluginUserWindow::showResource(const QUrl &url) {
    const QVariantMap resource = Resources::getResourceFromUrl(url.toString());
    
    if (resource.value("service") != m_user->service()) {
        QDesktopServices::openUrl(url);
        return;
    }
    
    if (resource.value("type") == Resources::PLAYLIST) {
        PluginPlaylistWindow *window = new PluginPlaylistWindow(m_user->service(), resource.value("id").toString(), this);
        window->show();
    }
    else if (resource.value("type") == Resources::USER) {
        PluginUserWindow *window = new PluginUserWindow(m_user->service(), resource.value("id").toString(), this);
        window->show();
    }
    else {
        PluginVideoWindow *window = new PluginVideoWindow(m_user->service(), resource.value("id").toString(), this);
        window->show();
    }
}

void PluginUserWindow::showWindow(const QModelIndex &index) {
    if (isBusy()) {
        return;
    }
    
    switch (index.row()) {
    case 0:
        showVideos();
        break;
    case 1:
        showPlaylists();
        break;
    default:
        break;
    }
}

void PluginUserWindow::onUserStatusChanged(ResourcesRequest::Status status) {
    switch (status) {
    case ResourcesRequest::Loading:
        showProgressIndicator();
        return;
    case ResourcesRequest::Ready:
        loadUserUi();
        break;
    case ResourcesRequest::Failed:
        QMessageBox::critical(this, tr("Error"), m_user->errorString());
        break;
    default:
        break;
    }
    
    hideProgressIndicator();
    disconnect(m_user, SIGNAL(statusChanged(ResourcesRequest::Status)),
               this, SLOT(onUserStatusChanged(ResourcesRequest::Status)));
}
