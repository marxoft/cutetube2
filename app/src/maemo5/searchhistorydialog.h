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

#ifndef SEARCHHISTORYDIALOG_H
#define SEARCHHISTORYDIALOG_H

#include "dialog.h"

class SearchHistoryModel;
class FilterBox;
class ListView;
class QHBoxLayout;
class QDialogButtonBox;
class QPushButton;
class QModelIndex;

class SearchHistoryDialog : public Dialog
{
    Q_OBJECT
    
public:
    explicit SearchHistoryDialog(QWidget *parent = 0);
    
protected:
    void keyPressEvent(QKeyEvent *e);
    
private Q_SLOTS:
    void chooseSearch(const QModelIndex &index);
    void removeSearch();
    
    void onCountChanged(int count);
    void onFilterTextChanged(const QString &text);
    
Q_SIGNALS:
    void searchChosen(const QString &query);
    
private:
    SearchHistoryModel *m_model;
    
    ListView *m_view;
    FilterBox *m_filterBox;
    QAction *m_removeAction;
    QDialogButtonBox *m_buttonBox;
    QPushButton *m_clearButton;
    QHBoxLayout *m_layout;
};
    
#endif // SEARCHHISTORYDIALOG_H
