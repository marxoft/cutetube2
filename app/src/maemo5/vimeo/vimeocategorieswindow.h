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

#ifndef VIMEOCATEGORIESWINDOW_H
#define VIMEOCATEGORIESWINDOW_H

#include "stackedwindow.h"
#include "vimeocategorymodel.h"

class ListView;
class QLabel;
class QVBoxLayout;

class VimeoCategoriesWindow : public StackedWindow
{
    Q_OBJECT
    
public:
    explicit VimeoCategoriesWindow(StackedWindow *parent = 0);
    
public Q_SLOTS:
    void list(const QString &resourcePath, const QVariantMap &filters = QVariantMap());
    
private Q_SLOTS:    
    void showCategory(const QModelIndex &index);
        
    void onModelStatusChanged(QVimeo::ResourcesRequest::Status status);
    
private:
    VimeoCategoryModel *m_model;
    
    ListView *m_view;
    QAction *m_reloadAction;
    QLabel *m_label;
    QVBoxLayout *m_layout;
};
    
#endif // VIMEOCATEGORIESWINDOW_H
