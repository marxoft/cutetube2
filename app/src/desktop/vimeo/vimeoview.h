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

#ifndef VIMEOVIEW_H
#define VIMEOVIEW_H

#include "pagestack.h"

class VimeoNavModel;
class QLabel;
class QListView;
class QStackedWidget;
class QSplitter;
class QVBoxLayout;

class VimeoView : public PageStack
{
    Q_OBJECT

public:
    explicit VimeoView(QWidget *parent = 0);

    virtual int count() const;

    virtual Page* currentPage() const;

public Q_SLOTS:
    void push(Page *page);
    void replace(Page *page);
    void pop();
    void clear();

private Q_SLOTS:
    void setCurrentPage(const QModelIndex &index);

private:
    void showSearchDialog();
    
    void listAccounts();
    void listCategories();
    void listFavourites();
    void listLatestVideos();
    void listPlaylists();
    void listSubscriptions();
    void listUploads();
    void listWatchLater();

    void searchUsers(const QString &query, const QString &order);
    void searchVideos(const QString &query, const QString &order);
    
    VimeoNavModel *m_model;
    
    QListView *m_view;

    QStackedWidget *m_stack;

    QSplitter *m_splitter;

    QVBoxLayout *m_layout;
};

#endif // VIMEOVIEW_H
