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

#ifndef DAILYMOTIONPLAYLISTWINDOW_H
#define DAILYMOTIONPLAYLISTWINDOW_H

#include "stackedwindow.h"
#include "dailymotionplaylist.h"

class DailymotionUser;
class DailymotionVideoDelegate;
class DailymotionVideoModel;
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

class DailymotionPlaylistWindow : public StackedWindow
{
    Q_OBJECT
    
public:
    explicit DailymotionPlaylistWindow(const QString &id, StackedWindow *parent = 0);
    explicit DailymotionPlaylistWindow(const DailymotionPlaylist *playlist, StackedWindow *parent = 0);
    ~DailymotionPlaylistWindow();
    
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
    
    void showContextMenu(const QPoint &pos);
    
    void showResource(const QUrl &url);
    void showUser();
    
    void onImageReady();
    void onModelStatusChanged(QDailymotion::ResourcesRequest::Status status);
    void onPlaylistStatusChanged(QDailymotion::ResourcesRequest::Status status);
    void onPlaylistUpdateStatusChanged(QDailymotion::ResourcesRequest::Status status);
    void onUserStatusChanged(QDailymotion::ResourcesRequest::Status status);
    void onVideoUpdateStatusChanged(QDailymotion::ResourcesRequest::Status status);
    
private:
    DailymotionPlaylist *m_playlist;
    DailymotionUser *m_user;
    DailymotionVideoModel *m_model;
    ImageCache *m_cache;
    
    PlaylistThumbnail *m_thumbnail;
    Image *m_avatar;
    ListView *m_view;
    DailymotionVideoDelegate *m_delegate;
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
    QAction *m_playlistAction;
    QAction *m_removeAction;
    QHBoxLayout *m_layout;
};
    
#endif // DAILYMOTIONPLAYLISTWINDOW_H
