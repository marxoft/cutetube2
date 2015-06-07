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

#ifndef PLUGINCATEGORIESWINDOW_H
#define PLUGINCATEGORIESWINDOW_H

#include "stackedwindow.h"
#include "plugincategorymodel.h"

class ListView;
class QLabel;
class QVBoxLayout;

class PluginCategoriesWindow : public StackedWindow
{
    Q_OBJECT
    
public:
    explicit PluginCategoriesWindow(StackedWindow *parent = 0);
    
public Q_SLOTS:
    void list(const QString &service, const QString &id = QString());
    
private Q_SLOTS:    
    void showCategory(const QModelIndex &index);
        
    void onModelStatusChanged(ResourcesRequest::Status status);
    
private:
    PluginCategoryModel *m_model;
    
    ListView *m_view;
    QAction *m_reloadAction;
    QLabel *m_label;
    QVBoxLayout *m_layout;
};
    
#endif // PLUGINCATEGORIESWINDOW_H
