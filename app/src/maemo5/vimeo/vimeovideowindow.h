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

#ifndef VIMEOVIDEOWINDOW_H
#define VIMEOVIDEOWINDOW_H

#include "stackedwindow.h"
#include "vimeocommentmodel.h"
#include "vimeovideomodel.h"

class VimeoUser;
class VideoDelegate;
class CommentDelegate;
class Image;
class ImageCache;
class ListView;
class TextBrowser;
class VideoThumbnail;
class QLabel;
class QScrollArea;
class QTabBar;
class QStackedWidget;
class QGridLayout;
class QModelIndex;

class VimeoVideoWindow : public StackedWindow
{
    Q_OBJECT
    
public:
    explicit VimeoVideoWindow(const QString &id, StackedWindow *parent = 0);
    explicit VimeoVideoWindow(const VimeoVideo *video, StackedWindow *parent = 0);
    ~VimeoVideoWindow();
    
private:
    void loadBaseUi();
    void loadUserUi();
    void loadVideoUi();
    
private Q_SLOTS:
    void getRelatedVideos();
    
    void addComment();
    void addVideoToPlaylist();
    void downloadVideo();
    void playVideo();
    void setVideoFavourite();
    void shareVideo();
    void watchVideoLater();
    
    void addRelatedVideoToPlaylist();
    void downloadRelatedVideo();
    void playRelatedVideo(const QModelIndex &index);
    void setRelatedVideoFavourite();
    void shareRelatedVideo();
    void showRelatedVideo(const QModelIndex &index);
    void watchRelatedVideoLater();
    
    void reload();
    
    void showContextMenu(const QPoint &pos);
    
    void showComments();
    void showRelatedVideos();
    void showResource(const QUrl &url);
    void showUser();
    void showUser(const QModelIndex &index);
    
    void onImageReady();
    void onCommentModelStatusChanged(QVimeo::ResourcesRequest::Status status);
    void onRelatedModelStatusChanged(QVimeo::ResourcesRequest::Status status);
    void onTabIndexChanged(int index);
    void onUserStatusChanged(QVimeo::ResourcesRequest::Status status);
    void onVideoStatusChanged(QVimeo::ResourcesRequest::Status status);
    void onVideoUpdateStatusChanged(QVimeo::ResourcesRequest::Status status);
        
private:
    VimeoVideo *m_video;
    VimeoUser *m_user;
    VimeoVideoModel *m_relatedModel;
    VimeoCommentModel *m_commentModel;
    ImageCache *m_cache;
    
    VideoThumbnail *m_thumbnail;
    Image *m_avatar;
    ListView *m_relatedView;
    ListView *m_commentView;
    VideoDelegate *m_relatedDelegate;
    CommentDelegate *m_commentDelegate;
    QScrollArea *m_scrollArea;
    QTabBar *m_tabBar;
    QStackedWidget *m_stack;
    QLabel *m_titleLabel;
    TextBrowser *m_descriptionLabel;
    QLabel *m_dateLabel;
    QLabel *m_userLabel;
    QLabel *m_noVideosLabel;
    QLabel *m_noCommentsLabel;
    QAction *m_reloadAction;
    QAction *m_downloadAction;
    QAction *m_shareAction;
    QAction *m_favouriteAction;
    QAction *m_watchLaterAction;
    QAction *m_playlistAction;
    QAction *m_commentAction;
    QMenu *m_contextMenu;
    QAction *m_relatedDownloadAction;
    QAction *m_relatedShareAction;
    QAction *m_relatedFavouriteAction;
    QAction *m_relatedWatchLaterAction;
    QAction *m_relatedPlaylistAction;
    QGridLayout *m_layout;
};
    
#endif // VIMEOVIDEOWINDOW_H
