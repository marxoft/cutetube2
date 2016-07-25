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

#ifndef YOUTUBEACCOUNTSWINDOW_H
#define YOUTUBEACCOUNTSWINDOW_H

#include "stackedwindow.h"

namespace QYouTube {
    class AuthenticationRequest;
    class ResourcesRequest;
}

class YouTubeAccountModel;
class ListView;
class QVBoxLayout;
class QModelIndex;
class QLabel;
class QAction;

class YouTubeAccountsWindow : public StackedWindow
{
    Q_OBJECT
    
public:
    explicit YouTubeAccountsWindow(StackedWindow *parent);
    
private:
    void initAuthRequest();
    void initUserRequest();
    
private Q_SLOTS:
    void selectAccount(const QModelIndex &index);
    
    void showAuthDialog();
    void revokeAccessToken();
    
    void onAuthRequestFinished();
    void onUserRequestFinished();
    void onModelCountChanged(int count);
    
private:
    QYouTube::AuthenticationRequest *m_authRequest;
    QYouTube::ResourcesRequest *m_userRequest;
    YouTubeAccountModel *m_model;
    
    ListView *m_view;
    QAction *m_addAction;
    QAction *m_removeAction;
    QLabel *m_label;
    QVBoxLayout *m_layout;
    
    bool m_revoke;
};
    
#endif // YOUTUBEACCOUNTSWINDOW_H
