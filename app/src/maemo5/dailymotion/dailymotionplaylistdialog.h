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

#ifndef DAILYMOTIONPLAYLISTDIALOG_H
#define DAILYMOTIONPLAYLISTDIALOG_H

#include "dialog.h"
#include "dailymotionplaylistmodel.h"

class DailymotionVideo;
class ImageCache;
class ListView;
class QTabBar;
class QStackedWidget;
class QLineEdit;
class QTextEdit;
class QLabel;
class QDialogButtonBox;
class QGridLayout;
class QModelIndex;

class DailymotionPlaylistDialog : public Dialog
{
    Q_OBJECT
    
public:
    explicit DailymotionPlaylistDialog(DailymotionVideo *video, QWidget *parent);
    ~DailymotionPlaylistDialog();
    
private:
    void getPlaylists();
    
    void showNewPlaylist();
    void showPlaylists();
    
private Q_SLOTS:
    void addToPlaylist(const QModelIndex &index);
    void createNewPlaylist();
    
    void onImageReady();
    void onModelStatusChanged(QDailymotion::ResourcesRequest::Status status);
    void onPlaylistStatusChanged(QDailymotion::ResourcesRequest::Status status);
    void onTabIndexChanged(int index);
    void onTitleChanged(const QString &text);
        
private:
    QPointer<DailymotionVideo> m_video;
    DailymotionPlaylist *m_playlist;
    DailymotionPlaylistModel *m_model;
    ImageCache *m_cache;
    
    ListView *m_view;
    QTabBar *m_tabBar;
    QStackedWidget *m_stack;
    QWidget *m_widget;
    QLineEdit *m_titleEdit;
    QTextEdit *m_descriptionEdit;
    QLabel *m_label;
    QDialogButtonBox *m_buttonBox;
    QGridLayout *m_layout;
};
    
#endif // DAILYMOTIONPLAYLISTDIALOG_H
