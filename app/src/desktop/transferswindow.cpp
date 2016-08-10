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

#include "transferswindow.h"
#include "customcommanddialog.h"
#include "definitions.h"
#include "settings.h"
#include "transferdelegate.h"
#include "transfermodel.h"
#include "transfers.h"
#include <QActionGroup>
#include <QHeaderView>
#include <QMenu>
#include <QMenuBar>
#include <QMessageBox>
#include <QToolBar>
#include <QTreeView>

TransfersWindow* TransfersWindow::self = 0;

TransfersWindow::TransfersWindow() :
    QMainWindow(),
    m_model(new TransferModel(this)),
    m_transferMenu(new QMenu(tr("&Transfer"), this)),
    m_categoryMenu(new QMenu(tr("&Category"), this)),
    m_priorityMenu(new QMenu(tr("&Priority"), this)),
    m_propertiesMenu(new QMenu(tr("&Properties"), this)),
    m_concurrentMenu(new QMenu(tr("Maximum &concurrent transfers"), this)),
    m_categoryGroup(new QActionGroup(this)),
    m_priorityGroup(new QActionGroup(this)),
    m_concurrentGroup(new QActionGroup(this)),
    m_startAction(new QAction(QIcon::fromTheme("media-playback-start"), tr("&Start all downloads"), this)),
    m_pauseAction(new QAction(QIcon::fromTheme("media-playback-pause"), tr("&Pause all downloads"), this)),
    m_propertiesAction(new QAction(QIcon::fromTheme("document-properties"), tr("&Properties"), this)),
    m_transferCommandAction(new QAction(QIcon::fromTheme("system-run"), tr("Set &custom command"), this)),
    m_transferStartAction(new QAction(QIcon::fromTheme("media-playback-start"), tr("&Start"), this)),
    m_transferPauseAction(new QAction(QIcon::fromTheme("media-playback-pause"), tr("&Pause"), this)),
    m_transferRemoveAction(new QAction(QIcon::fromTheme("edit-delete"), tr("&Remove"), this)),
    m_topToolBar(new QToolBar(this)),
    m_bottomToolBar(new QToolBar(this)),
    m_view(new QTreeView(this))
{
    setWindowTitle(tr("Transfers"));
    setCentralWidget(m_view);
    addToolBar(Qt::TopToolBarArea, m_topToolBar);
    addToolBar(Qt::BottomToolBarArea, m_bottomToolBar);
    
    menuBar()->addMenu(m_transferMenu);
    
    m_transferMenu->addAction(m_transferCommandAction);
    m_transferMenu->addAction(m_transferStartAction);
    m_transferMenu->addAction(m_transferPauseAction);
    m_transferMenu->addMenu(m_categoryMenu);
    m_transferMenu->addMenu(m_priorityMenu);
    m_transferMenu->addAction(m_transferRemoveAction);
    m_transferMenu->setEnabled(false);
    
    setCategoryMenuActions();
    
    const QStringList priorities = QStringList() << tr("High") << tr("Normal") << tr("Low");
    
    for (int i = 0; i < priorities.size(); i++) {
        QAction *action = m_priorityMenu->addAction(priorities.at(i), this, SLOT(setTransferPriority()));
        action->setCheckable(true);
        action->setData(i);
        m_priorityGroup->addAction(action);
    }
    
    const int max = Settings::maximumConcurrentTransfers();
    
    for (int i = 1; i <= MAX_CONCURRENT_TRANSFERS; i++) {
        QAction *action = m_concurrentMenu->addAction(QString::number(i), this, SLOT(setMaximumConcurrentTransfers()));
        action->setCheckable(true);
        action->setChecked(i == max);
        action->setData(i);
        m_concurrentGroup->addAction(action);
    }
    
    m_propertiesMenu->addMenu(m_concurrentMenu);
    
    m_topToolBar->setObjectName("transfersTopToolBar");
    m_topToolBar->setWindowTitle(tr("Top toolbar"));
    m_topToolBar->setAllowedAreas(Qt::TopToolBarArea);
    m_topToolBar->setMovable(false);
    m_topToolBar->addAction(m_startAction);
    m_topToolBar->addAction(m_pauseAction);
    
    m_bottomToolBar->setObjectName("transfersBottomToolBar");
    m_bottomToolBar->setWindowTitle(tr("Bottom toolbar"));
    m_bottomToolBar->setAllowedAreas(Qt::BottomToolBarArea);
    m_bottomToolBar->setMovable(false);
    m_bottomToolBar->addAction(m_propertiesAction);
    
    m_view->setModel(m_model);
    m_view->setItemDelegate(new TransferDelegate(m_view));
    m_view->setAlternatingRowColors(true);
    m_view->setSelectionBehavior(QTreeView::SelectRows);
    m_view->setContextMenuPolicy(Qt::CustomContextMenu);
    m_view->setEditTriggers(QTreeView::NoEditTriggers);
    m_view->setItemsExpandable(false);
    m_view->setUniformRowHeights(true);
    m_view->setAllColumnsShowFocus(true);
    m_view->setRootIsDecorated(false);
    m_view->header()->restoreState(Settings::transfersHeaderViewState());
    
    connect(m_categoryMenu, SIGNAL(aboutToShow()), this, SLOT(setActiveCategoryMenuAction()));
    connect(m_priorityMenu, SIGNAL(aboutToShow()), this, SLOT(setActivePriorityMenuAction()));
    connect(m_startAction, SIGNAL(triggered()), Transfers::instance(), SLOT(start()));
    connect(m_pauseAction, SIGNAL(triggered()), Transfers::instance(), SLOT(pause()));
    connect(m_propertiesAction, SIGNAL(triggered()), this, SLOT(showPropertiesMenu()));
    connect(m_transferCommandAction, SIGNAL(triggered()), this, SLOT(setTransferCustomCommand()));
    connect(m_transferStartAction, SIGNAL(triggered()), this, SLOT(queueTransfer()));
    connect(m_transferPauseAction, SIGNAL(triggered()), this, SLOT(pauseTransfer()));
    connect(m_transferRemoveAction, SIGNAL(triggered()), this, SLOT(removeTransfer()));
    connect(m_view, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(showContextMenu(QPoint)));
    connect(m_view->selectionModel(), SIGNAL(currentRowChanged(QModelIndex, QModelIndex)),
            this, SLOT(onCurrentTransferChanged(QModelIndex)));
    connect(Settings::instance(), SIGNAL(categoriesChanged()), this, SLOT(setCategoryMenuActions()));
    connect(Settings::instance(), SIGNAL(maximumConcurrentTransfersChanged(int)),
            this, SLOT(onMaximumConcurrentTransfersChanged(int)));
    
    restoreGeometry(Settings::transfersWindowGeometry());
    restoreState(Settings::transfersWindowState());
}

TransfersWindow::~TransfersWindow() {
    self = 0;
}

void TransfersWindow::closeEvent(QCloseEvent *event) {
    Settings::setTransfersHeaderViewState(m_view->header()->saveState());
    Settings::setTransfersWindowGeometry(saveGeometry());
    Settings::setTransfersWindowState(saveState());
    QMainWindow::closeEvent(event);
}

TransfersWindow* TransfersWindow::instance() {
    return self ? self : self = new TransfersWindow;
}

void TransfersWindow::queueTransfer() {
    if (m_view->currentIndex().isValid()) {
        queueTransfer(m_view->currentIndex());
    }
}

void TransfersWindow::queueTransfer(const QModelIndex &index) {
    if (Transfer *transfer = Transfers::instance()->get(index.row())) {
        transfer->queue();
    }
}

void TransfersWindow::pauseTransfer() {
    if (m_view->currentIndex().isValid()) {
        pauseTransfer(m_view->currentIndex());
    }
}

void TransfersWindow::pauseTransfer(const QModelIndex &index) {
    if (Transfer *transfer = Transfers::instance()->get(index.row())) {
        transfer->pause();
    }
}

void TransfersWindow::removeTransfer() {
    if (m_view->currentIndex().isValid()) {
        removeTransfer(m_view->currentIndex());
    }
}

void TransfersWindow::removeTransfer(const QModelIndex &index) {
    if (Transfer *transfer = Transfers::instance()->get(index.row())) {
        if (QMessageBox::question(this, tr("Remove?"),
            tr("Do you want to remove transfer '%1'?").arg(transfer->title())) == QMessageBox::Yes) {
            transfer->cancel();
        }
    }
}

void TransfersWindow::setTransferCategory() {
    if (m_view->currentIndex().isValid()) {
        if (const QAction *action = m_categoryGroup->checkedAction()) {
            setTransferCategory(m_view->currentIndex(), action->text());
        }
    }
}

void TransfersWindow::setTransferCategory(const QModelIndex &index, const QString &category) {
    m_model->setData(index, category, TransferModel::CategoryRole);
}

void TransfersWindow::setTransferCustomCommand() {
    if (m_view->currentIndex().isValid()) {
        CustomCommandDialog dialog(this);
        
        if (dialog.exec() == QDialog::Accepted) {
            setTransferCustomCommand(m_view->currentIndex(), dialog.command(), dialog.overrideEnabled());
        }
    }
}

void TransfersWindow::setTransferCustomCommand(const QModelIndex &index, const QString &command, bool overrideEnabled) {
    m_model->setData(index, command, TransferModel::CustomCommandRole);
    m_model->setData(index, overrideEnabled, TransferModel::CustomCommandOverrideEnabledRole);
}

void TransfersWindow::setTransferPriority() {
    if (m_view->currentIndex().isValid()) {
        if (const QAction *action = m_priorityGroup->checkedAction()) {
            setTransferPriority(m_view->currentIndex(), action->data().toInt());
        }
    }
}

void TransfersWindow::setTransferPriority(const QModelIndex &index, int priority) {
    m_model->setData(index, priority, TransferModel::PriorityRole);
}

void TransfersWindow::setCategoryMenuActions() {
    const QStringList categories = Settings::categoryNames();
    m_categoryMenu->clear();
    
    foreach (const QString &category, categories) {
        QAction *action = m_categoryMenu->addAction(category, this, SLOT(setTransferCategory()));
        action->setCheckable(true);
        action->setData(category);
        m_categoryGroup->addAction(action);
    }
}

void TransfersWindow::setActiveCategoryMenuAction() {
    if (!m_view->currentIndex().isValid()) {
        return;
    }
    
    const QVariant category = m_view->currentIndex().data(TransferModel::CategoryRole).toString();
    
    foreach (QAction *action, m_categoryGroup->actions()) {
        if (action->data() == category) {
            action->setChecked(true);
            break;
        }
    }
}

void TransfersWindow::setActivePriorityMenuAction() {
    if (!m_view->currentIndex().isValid()) {
        return;
    }
    
    const QVariant priority = m_view->currentIndex().data(TransferModel::PriorityRole);
    
    foreach (QAction *action, m_priorityGroup->actions()) {
        if (action->data() == priority) {
            action->setChecked(true);
            break;
        }
    }
}

void TransfersWindow::setMaximumConcurrentTransfers() {
    if (const QAction *action = m_concurrentGroup->checkedAction()) {
        Settings::setMaximumConcurrentTransfers(action->data().toInt());
    }
}

void TransfersWindow::showContextMenu(const QPoint &pos) {
    if (m_view->currentIndex().isValid()) {
        m_transferMenu->popup(m_view->mapToGlobal(pos));
    }
}

void TransfersWindow::showPropertiesMenu() {
    m_propertiesMenu->popup(QCursor::pos());
}

void TransfersWindow::onCurrentTransferChanged(const QModelIndex &index) {
    m_transferMenu->setEnabled(index.isValid());
}

void TransfersWindow::onMaximumConcurrentTransfersChanged(int maximum) {
    foreach (QAction *action, m_concurrentGroup->actions()) {
        if (action->data() == maximum) {
            action->setChecked(true);
            break;
        }
    }
}
