/*
 * Copyright (C) 2014 Stuart Howarth <showarth@marxoft.co.uk>
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms and conditions of the GNU Lesser General Public License,
 * version 3, as published by the Free Software Foundation.
 *
 * This program is distributed in the hope it will be useful, but WITHOUT ANY
 * WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for
 * more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program; if not, write to the Free Software Foundation,
 * Inc., 51 Franklin St - Fifth Floor, Boston, MA 02110-1301 USA.
 */

#ifndef NEWCATEGORYDIALOG_H
#define NEWCATEGORYDIALOG_H

#include "dialog.h"

class QLineEdit;
class QMaemo5ValueButton;

class NewCategoryDialog : public Dialog
{
    Q_OBJECT

public:
    explicit NewCategoryDialog(QWidget *parent = 0);

public Q_SLOTS:
    void setName(const QString &name);
    void setPath(const QString &path);

private Q_SLOTS:
    void addCategory();
    
    void showFileDialog();
    
    void onNameTextChanged(const QString &text);
    
private:
    QString m_path;
    
    QLineEdit *m_nameEdit;
    QMaemo5ValueButton *m_pathSelector;
    QPushButton *m_doneButton;
};

#endif // NEWCATEGORYDIALOG_H
