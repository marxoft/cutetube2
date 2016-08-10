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

#ifndef TRANSFERSWINDOW_H
#define TRANSFERSWINDOW_H

#include <QMainWindow>

class TransferModel;
class QActionGroup;
class QTreeView;

class TransfersWindow : public QMainWindow
{
    Q_OBJECT

public:
    ~TransfersWindow();
    
    static TransfersWindow* instance();
    
private Q_SLOTS:
    void queueTransfer();
    void queueTransfer(const QModelIndex &index);
    
    void pauseTransfer();
    void pauseTransfer(const QModelIndex &index);
    
    void removeTransfer();
    void removeTransfer(const QModelIndex &index);
    
    void setTransferCategory();
    void setTransferCategory(const QModelIndex &index, const QString &category);
    
    void setTransferCustomCommand();
    void setTransferCustomCommand(const QModelIndex &index, const QString &command, bool overrideEnabled);
    
    void setTransferPriority();
    void setTransferPriority(const QModelIndex &index, int priority);
    
    void setCategoryMenuActions();
    
    void setActiveCategoryMenuAction();
    void setActivePriorityMenuAction();
    
    void setMaximumConcurrentTransfers();
    
    void showContextMenu(const QPoint &pos);
    void showPropertiesMenu();
    
    void onCurrentTransferChanged(const QModelIndex &index);
    void onMaximumConcurrentTransfersChanged(int maximum);

private:
    TransfersWindow();
    
    virtual void closeEvent(QCloseEvent *event);
        
    static TransfersWindow *self;
    
    TransferModel *m_model;
    
    QMenu *m_transferMenu;
    QMenu *m_categoryMenu;
    QMenu *m_priorityMenu;
    QMenu *m_propertiesMenu;
    QMenu *m_concurrentMenu;
    
    QActionGroup *m_categoryGroup;
    QActionGroup *m_priorityGroup;
    QActionGroup *m_concurrentGroup;
    
    QAction *m_startAction;
    QAction *m_pauseAction;
    QAction *m_propertiesAction;
    
    QAction *m_transferCommandAction;
    QAction *m_transferStartAction;
    QAction *m_transferPauseAction;
    QAction *m_transferRemoveAction;
    
    QToolBar *m_topToolBar;
    QToolBar *m_bottomToolBar;
        
    QTreeView *m_view;
};

#endif // TRANSFERSWINDOW_H
