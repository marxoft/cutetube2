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

#ifndef YOUTUBEPLAYLISTDIALOG_H
#define YOUTUBEPLAYLISTDIALOG_H

#include "dialog.h"
#include "youtubeplaylistmodel.h"

class YouTubeVideo;
class ImageCache;
class ListView;
class QTabBar;
class QStackedWidget;
class QScrollArea;
class QLineEdit;
class QTextEdit;
class QCheckBox;
class QLabel;
class QDialogButtonBox;
class QGridLayout;
class QModelIndex;

class YouTubePlaylistDialog : public Dialog
{
    Q_OBJECT
    
public:
    explicit YouTubePlaylistDialog(const YouTubeVideo *video, QWidget *parent);
    ~YouTubePlaylistDialog();
    
protected:
    void showEvent(QShowEvent *e);
    
private:
    void getPlaylists();
    
    void showNewPlaylist();
    void showPlaylists();
    
private Q_SLOTS:
    void addToPlaylist(const QModelIndex &index);
    void createNewPlaylist();
    
    void onImageReady();
    void onModelStatusChanged(QYouTube::ResourcesRequest::Status status);
    void onPlaylistStatusChanged(QYouTube::ResourcesRequest::Status status);
    void onTabIndexChanged(int index);
    void onTitleChanged(const QString &text);
        
private:
    const YouTubeVideo *m_video;
    YouTubePlaylist *m_playlist;
    YouTubePlaylistModel *m_model;
    ImageCache *m_cache;
    
    ListView *m_view;
    QTabBar *m_tabBar;
    QStackedWidget *m_stack;
    QScrollArea *m_scrollArea;
    QLineEdit *m_titleEdit;
    QTextEdit *m_descriptionEdit;
    QCheckBox *m_privateCheckBox;
    QLabel *m_label;
    QDialogButtonBox *m_buttonBox;
    QGridLayout *m_layout;
};
    
#endif // YOUTUBEPLAYLISTDIALOG_H
