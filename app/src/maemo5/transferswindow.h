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

#ifndef TRANSFERSWINDOW_H
#define TRANSFERSWINDOW_H

#include "stackedwindow.h"

class TransferModel;
class QAction;
class QActionGroup;
class QTreeView;
class QMenu;
class QLabel;
class QVBoxLayout;

class TransfersWindow : public StackedWindow
{
    Q_OBJECT
    
public:
    explicit TransfersWindow(StackedWindow *parent = 0);
    
private Q_SLOTS:
    void onCountChanged(int count);
    void setCategoryMenuActions();
    void setTransferMenuActions();
    void showContextMenu(const QPoint &pos);
    void startCurrentTransfer();
    void pauseCurrentTransfer();
    void removeCurrentTransfer();
    void setConvertCurrentTransferToAudio();
    void setCurrentTransferCategory();
    void setCurrentTransferPriority();
    
private:
    TransferModel *m_model;
    
    QTreeView *m_view;
    QAction *m_startAction;
    QAction *m_pauseAction;
    QMenu *m_contextMenu;
    QAction *m_transferConvertToAudioAction;
    QAction *m_transferStartAction;
    QAction *m_transferPauseAction;
    QMenu *m_transferCategoryMenu;
    QActionGroup *m_transferCategoryGroup;
    QMenu *m_transferPriorityMenu;
    QActionGroup *m_transferPriorityGroup;
    QAction *m_transferHighPriorityAction;
    QAction *m_transferNormalPriorityAction;
    QAction *m_transferLowPriorityAction;
    QAction *m_transferRemoveAction;
    QLabel *m_label;
    QVBoxLayout *m_layout;
};

#endif // TRANSFERSWINDOW_H
