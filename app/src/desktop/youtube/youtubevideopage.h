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

#ifndef YOUTUBEVIDEOPAGE_H
#define YOUTUBEVIDEOPAGE_H

#include "page.h"
#include "youtubevideo.h"

class Image;
class TextBrowser;
class VideoThumbnail;
class YouTubeCommentsPage;
class YouTubeVideosPage;
class QGridLayout;
class QLabel;
class QSplitter;
class QStackedWidget;
class QTabBar;
class QToolBar;
class QVBoxLayout;

class YouTubeVideoPage : public Page
{
    Q_OBJECT

public:
    explicit YouTubeVideoPage(const QString &videoId, QWidget *parent = 0);
    explicit YouTubeVideoPage(const YouTubeVideo *video, QWidget *parent = 0);
    
    virtual Status status() const;
    virtual QString statusText() const;

public Q_SLOTS:
    virtual void cancel();
    virtual void reload();

protected:
    virtual void resizeEvent(QResizeEvent *event);

private Q_SLOTS:
    void setCurrentTab(int index);
    
    void showRelatedTab();
    void showCommentsTab();
        
    void onVideoLoadingStatusChanged(QYouTube::ResourcesRequest::Status status);
    
private:
    void setStatus(Status s);
    void setStatusText(const QString &text);
        
    void loadUi();    
    
    YouTubeVideo *m_video;
    
    QSplitter *m_splitter;
    
    QWidget *m_videoContainer;
    
    VideoThumbnail *m_thumbnail;
    
    QToolBar *m_toolBar;
    
    QAction *m_likeAction;
    QAction *m_dislikeAction;
    QAction *m_favouriteAction;
    QAction *m_playlistAction;
    QAction *m_queueAction;
    QAction *m_downloadAction;
    
    Image *m_avatar;
    
    QLabel *m_usernameLabel;
    QLabel *m_dateLabel;
    
    TextBrowser *m_descriptionLabel;
    
    QGridLayout *m_videoLayout;
    
    QWidget *m_tabsContainer;
    
    QTabBar *m_tabBar;
    
    QStackedWidget *m_tabs;
        
    YouTubeVideosPage *m_relatedTab;
    YouTubeCommentsPage *m_commentsTab;
    
    QVBoxLayout *m_tabsLayout;
    
    QVBoxLayout *m_layout;
    
    Status m_status;
    
    QString m_statusText;
};

#endif // YOUTUBEVIDEOPAGE_H
