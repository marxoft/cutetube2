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

#ifndef PLUGINPLAYLISTWINDOW_H
#define PLUGINPLAYLISTWINDOW_H

#include "stackedwindow.h"
#include "pluginplaylist.h"

class PluginUser;
class VideoDelegate;
class PluginVideoModel;
class Image;
class ImageCache;
class ListView;
class TextBrowser;
class PlaylistThumbnail;
class QLabel;
class QScrollArea;
class QHBoxLayout;
class QModelIndex;

class PluginPlaylistWindow : public StackedWindow
{
    Q_OBJECT
    
public:
    explicit PluginPlaylistWindow(const QString &service, const QString &id, StackedWindow *parent = 0);
    explicit PluginPlaylistWindow(const PluginPlaylist *playlist, StackedWindow *parent = 0);
    ~PluginPlaylistWindow();
    
private:
    void loadBaseUi();
    void loadPlaylistUi();
    void loadUserUi();
    
private Q_SLOTS:
    void getVideos();
    
    void playPlaylist();
    
    void downloadVideo();
    void playVideo(const QModelIndex &index);
    void shareVideo();
    void showVideo(const QModelIndex &index);
    
    void showContextMenu(const QPoint &pos);
    
    void showResource(const QUrl &url);
    void showUser();
    
    void onImageReady();
    void onModelStatusChanged(ResourcesRequest::Status status);
    void onPlaylistStatusChanged(ResourcesRequest::Status status);
    void onUserStatusChanged(ResourcesRequest::Status status);
    
private:
    PluginPlaylist *m_playlist;
    PluginUser *m_user;
    PluginVideoModel *m_model;
    ImageCache *m_cache;
    
    PlaylistThumbnail *m_thumbnail;
    Image *m_avatar;
    ListView *m_view;
    VideoDelegate *m_delegate;
    QScrollArea *m_scrollArea;
    QLabel *m_titleLabel;
    TextBrowser *m_descriptionLabel;
    QLabel *m_dateLabel;
    QLabel *m_userLabel;
    QLabel *m_noVideosLabel;
    QAction *m_reloadAction;
    QMenu *m_contextMenu;
    QAction *m_downloadAction;
    QAction *m_shareAction;
    QHBoxLayout *m_layout;
};
    
#endif // PLUGINPLAYLISTWINDOW_H
