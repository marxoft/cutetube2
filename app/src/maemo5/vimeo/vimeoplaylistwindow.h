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

#ifndef VIMEOPLAYLISTWINDOW_H
#define VIMEOPLAYLISTWINDOW_H

#include "stackedwindow.h"
#include "vimeoplaylist.h"

class VimeoUser;
class VideoDelegate;
class VimeoVideoModel;
class Image;
class ImageCache;
class ListView;
class TextBrowser;
class PlaylistThumbnail;
class QLabel;
class QActionGroup;
class QScrollArea;
class QHBoxLayout;
class QModelIndex;

class VimeoPlaylistWindow : public StackedWindow
{
    Q_OBJECT
    
public:
    explicit VimeoPlaylistWindow(const QString &id, StackedWindow *parent = 0);
    explicit VimeoPlaylistWindow(const VimeoPlaylist *playlist, StackedWindow *parent = 0);
    ~VimeoPlaylistWindow();
    
private:
    void loadBaseUi();
    void loadPlaylistUi();
    void loadUserUi();
    
private Q_SLOTS:
    void enableGridMode();
    void enableListMode();
    
    void getVideos();
    
    void playPlaylist();
    
    void addVideoToPlaylist();
    void downloadVideo();
    void playVideo(const QModelIndex &index);
    void removeVideo();
    void setVideoFavourite();
    void shareVideo();
    void showVideo(const QModelIndex &index);
    void watchVideoLater();
    
    void showContextMenu(const QPoint &pos);
    
    void showResource(const QUrl &url);
    void showUser();
    
    void onImageReady();
    void onModelStatusChanged(QVimeo::ResourcesRequest::Status status);
    void onPlaylistStatusChanged(QVimeo::ResourcesRequest::Status status);
    void onPlaylistUpdateStatusChanged(QVimeo::ResourcesRequest::Status status);
    void onUserStatusChanged(QVimeo::ResourcesRequest::Status status);
    void onVideoUpdateStatusChanged(QVimeo::ResourcesRequest::Status status);
    
private:
    VimeoPlaylist *m_playlist;
    VimeoUser *m_user;
    VimeoVideoModel *m_model;
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
    QActionGroup *m_viewGroup;
    QAction *m_listAction;
    QAction *m_gridAction;
    QAction *m_reloadAction;
    QMenu *m_contextMenu;
    QAction *m_downloadAction;
    QAction *m_shareAction;
    QAction *m_favouriteAction;
    QAction *m_watchLaterAction;
    QAction *m_playlistAction;
    QAction *m_removeAction;
    QHBoxLayout *m_layout;
};
    
#endif // VIMEOPLAYLISTWINDOW_H
