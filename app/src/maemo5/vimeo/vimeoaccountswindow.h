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

#ifndef VIMEOACCOUNTSWINDOW_H
#define VIMEOACCOUNTSWINDOW_H

#include "stackedwindow.h"

namespace QVimeo {
    class AuthenticationRequest;
}

class VimeoAccountModel;
class ListView;
class QVBoxLayout;
class QModelIndex;
class QLabel;
class QAction;

class VimeoAccountsWindow : public StackedWindow
{
    Q_OBJECT
    
public:
    explicit VimeoAccountsWindow(StackedWindow *parent);
    
private:
    void initAuthRequest();
    
private Q_SLOTS:
    void selectAccount(const QModelIndex &index);
    
    void showAuthDialog();
    void submitCode(const QString &code);
    void removeAccount();
    
    void onAuthRequestFinished();
    void onModelCountChanged(int count);
    
private:
    QVimeo::AuthenticationRequest *m_authRequest;
    VimeoAccountModel *m_model;
    
    ListView *m_view;
    QAction *m_addAction;
    QAction *m_removeAction;
    QLabel *m_label;
    QVBoxLayout *m_layout;
};
    
#endif // VIMEOACCOUNTSWINDOW_H
