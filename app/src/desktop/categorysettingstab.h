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

#ifndef CATEGORYSETTINGSTAB_H
#define CATEGORYSETTINGSTAB_H

#include "settingstab.h"

class CategoryModel;
class QFormLayout;
class QLineEdit;
class QPushButton;
class QTreeView;

class CategorySettingsTab : public SettingsTab
{
    Q_OBJECT

public:
    explicit CategorySettingsTab(QWidget *parent = 0);

private Q_SLOTS:
    void addCategory();
    void setCurrentCategory(const QModelIndex &index);
    
    void showContextMenu(const QPoint &pos);
    void showFileDialog();

    void onNameChanged(const QString &name);
    void onPathChanged(const QString &path);

private:
    CategoryModel *m_model;
    
    QTreeView *m_view;

    QLineEdit *m_nameEdit;
    QLineEdit *m_pathEdit;

    QPushButton *m_pathButton;
    QPushButton *m_saveButton;

    QFormLayout *m_layout;
};

#endif // CATEGORYSETTINGSTAB_H
