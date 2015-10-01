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

#ifndef DAILYMOTIONUSERSWINDOW_H
#define DAILYMOTIONUSERSWINDOW_H

#include "stackedwindow.h"
#include "dailymotionusermodel.h"

class UserDelegate;
class ImageCache;
class ListView;
class QLabel;
class QVBoxLayout;

class DailymotionUsersWindow : public StackedWindow
{
    Q_OBJECT
    
public:
    explicit DailymotionUsersWindow(StackedWindow *parent = 0);
    ~DailymotionUsersWindow();
    
public Q_SLOTS:
    void list(const QString &resourcePath, const QVariantMap &filters = QVariantMap());
    
private Q_SLOTS:
    void showUser(const QModelIndex &index);
        
    void onImageReady();
    void onModelStatusChanged(QDailymotion::ResourcesRequest::Status status);
    
private:
    DailymotionUserModel *m_model;
    ImageCache *m_cache;
    
    ListView *m_view;
    UserDelegate *m_delegate;
    QAction *m_reloadAction;
    QLabel *m_label;
    QVBoxLayout *m_layout;
};
    
#endif // DAILYMOTIONUSERSWINDOW_H
