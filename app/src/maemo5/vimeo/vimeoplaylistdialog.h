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

#ifndef VIMEOPLAYLISTDIALOG_H
#define VIMEOPLAYLISTDIALOG_H

#include "dialog.h"
#include "vimeoplaylistmodel.h"

class VimeoVideo;
class ImageCache;
class ListView;
class QTabBar;
class QStackedWidget;
class QScrollArea;
class QLineEdit;
class QTextEdit;
class QLabel;
class QDialogButtonBox;
class QGridLayout;
class QModelIndex;

class VimeoPlaylistDialog : public Dialog
{
    Q_OBJECT
    
public:
    explicit VimeoPlaylistDialog(const VimeoVideo *video, QWidget *parent);
    ~VimeoPlaylistDialog();
    
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
    void onModelStatusChanged(QVimeo::ResourcesRequest::Status status);
    void onPlaylistStatusChanged(QVimeo::ResourcesRequest::Status status);
    void onTabIndexChanged(int index);
    void onTitleChanged(const QString &text);
        
private:
    const VimeoVideo *m_video;
    VimeoPlaylist *m_playlist;
    VimeoPlaylistModel *m_model;
    ImageCache *m_cache;
    
    ListView *m_view;
    QTabBar *m_tabBar;
    QStackedWidget *m_stack;
    QScrollArea *m_scrollArea;
    QLineEdit *m_titleEdit;
    QTextEdit *m_descriptionEdit;
    QLineEdit *m_passwordEdit;
    QLabel *m_label;
    QDialogButtonBox *m_buttonBox;
    QGridLayout *m_layout;
};
    
#endif // VIMEOPLAYLISTDIALOG_H
