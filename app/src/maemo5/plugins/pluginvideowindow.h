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

#ifndef PLUGINVIDEOWINDOW_H
#define PLUGINVIDEOWINDOW_H

#include "stackedwindow.h"
#include "plugincommentmodel.h"
#include "pluginvideomodel.h"

class PluginUser;
class PluginVideoDelegate;
class PluginCommentDelegate;
class Image;
class ImageCache;
class ListView;
class TextBrowser;
class VideoThumbnail;
class QLabel;
class QActionGroup;
class QScrollArea;
class QTabBar;
class QStackedWidget;
class QGridLayout;
class QModelIndex;

class PluginVideoWindow : public StackedWindow
{
    Q_OBJECT
    
public:
    explicit PluginVideoWindow(const QString &service, const QString &id, StackedWindow *parent = 0);
    explicit PluginVideoWindow(const PluginVideo *video, StackedWindow *parent = 0);
    ~PluginVideoWindow();
    
private:
    void loadBaseUi();
    void loadUserUi();
    void loadVideoUi();
    
private Q_SLOTS:
    void enableGridMode();
    void enableListMode();
    
    void getRelatedVideos();
    
    void downloadVideo();
    void playVideo();
    void shareVideo();
    
    void downloadRelatedVideo();
    void playRelatedVideo(const QModelIndex &index);
    void shareRelatedVideo();
    void showRelatedVideo(const QModelIndex &index);
    
    void reload();
    
    void showContextMenu(const QPoint &pos);
    
    void showComments();
    void showRelatedVideos();
    void showResource(const QUrl &url);
    void showUser();
    void showUser(const QModelIndex &index);
    
    void onImageReady();
    void onCommentModelStatusChanged(ResourcesRequest::Status status);
    void onRelatedModelStatusChanged(ResourcesRequest::Status status);
    void onTabIndexChanged(int index);
    void onUserStatusChanged(ResourcesRequest::Status status);
    void onVideoStatusChanged(ResourcesRequest::Status status);
        
private:
    PluginVideo *m_video;
    PluginUser *m_user;
    PluginVideoModel *m_relatedModel;
    PluginCommentModel *m_commentModel;
    ImageCache *m_cache;
    
    VideoThumbnail *m_thumbnail;
    Image *m_avatar;
    ListView *m_relatedView;
    ListView *m_commentView;
    PluginVideoDelegate *m_relatedDelegate;
    PluginCommentDelegate *m_commentDelegate;
    QScrollArea *m_scrollArea;
    QTabBar *m_tabBar;
    QStackedWidget *m_stack;
    QLabel *m_titleLabel;
    TextBrowser *m_descriptionLabel;
    QLabel *m_dateLabel;
    QLabel *m_userLabel;
    QLabel *m_noVideosLabel;
    QLabel *m_noCommentsLabel;
    QActionGroup *m_viewGroup;
    QAction *m_listAction;
    QAction *m_gridAction;
    QAction *m_reloadAction;
    QAction *m_downloadAction;
    QAction *m_shareAction;
    QMenu *m_contextMenu;
    QAction *m_relatedDownloadAction;
    QAction *m_relatedShareAction;
    QGridLayout *m_layout;
};
    
#endif // PLUGINVIDEOWINDOW_H
