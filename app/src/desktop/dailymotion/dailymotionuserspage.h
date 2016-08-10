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

#ifndef DAILYMOTIONUSERSPAGE_H
#define DAILYMOTIONUSERSPAGE_H

#include "page.h"
#include "dailymotionusermodel.h"

class ImageCache;
class UserDelegate;
class QListView;
class QVBoxLayout;

class DailymotionUsersPage : public Page
{
    Q_OBJECT

public:
    explicit DailymotionUsersPage(QWidget *parent = 0);
    ~DailymotionUsersPage();

    Status status() const;

public Q_SLOTS:
    virtual void cancel();
    virtual void reload();
    
    void list(const QString &resourcePath, const QVariantMap &filters = QVariantMap());

private Q_SLOTS:    
    void onImageReady();
    void onModelStatusChanged(QDailymotion::ResourcesRequest::Status status);

private:
    void setStatus(Status s);
    
    DailymotionUserModel *m_model;

    ImageCache *m_cache;

    UserDelegate *m_delegate;
    
    QListView *m_view;

    QVBoxLayout *m_layout;

    Status m_status;
};

#endif // DAILYMOTIONUSERSPAGE_H
