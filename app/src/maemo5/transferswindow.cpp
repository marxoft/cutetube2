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

#include "transferswindow.h"
#include "settings.h"
#include "transfermodel.h"
#include "transfers.h"
#include <QActionGroup>
#include <QTreeView>
#include <QHeaderView>
#include <QMenu>
#include <QMenuBar>
#include <QLabel>
#include <QVBoxLayout>

TransfersWindow::TransfersWindow(StackedWindow *parent) :
    StackedWindow(parent),
    m_model(new TransferModel(this)),
    m_view(new QTreeView(this)),
    m_startAction(new QAction(tr("Start all transfers"), this)),
    m_pauseAction(new QAction(tr("Pause all transfers"), this)),
    m_contextMenu(new QMenu(this)),
    m_transferConvertToAudioAction(m_contextMenu->addAction(tr("Convert to audio file"),
                                                             this, SLOT(setConvertCurrentTransferToAudio()))),
    m_transferStartAction(m_contextMenu->addAction(QIcon::fromTheme("media-playback-start"), tr("Start"),
                                                    this, SLOT(startCurrentTransfer()))),
    m_transferPauseAction(m_contextMenu->addAction(QIcon::fromTheme("media-playback-pause"), tr("Pause"),
                                                    this, SLOT(pauseCurrentTransfer()))),
    m_transferCategoryMenu(m_contextMenu->addMenu(tr("Category"))),
    m_transferCategoryGroup(new QActionGroup(this)),
    m_transferPriorityMenu(m_contextMenu->addMenu(tr("Priority"))),
    m_transferPriorityGroup(new QActionGroup(this)),
    m_transferHighPriorityAction(m_transferPriorityMenu->addAction(tr("High"),
                                                                   this, SLOT(setCurrentTransferPriority()))),
    m_transferNormalPriorityAction(m_transferPriorityMenu->addAction(tr("Normal"),
                                                                     this, SLOT(setCurrentTransferPriority()))),
    m_transferLowPriorityAction(m_transferPriorityMenu->addAction(tr("Low"),
                                                                  this, SLOT(setCurrentTransferPriority()))),
    m_transferRemoveAction(m_contextMenu->addAction(QIcon::fromTheme("edit-delete"), tr("Remove"),
                                                     this, SLOT(removeCurrentTransfer()))),
    m_label(new QLabel(QString("<p align='center'; style='font-size: 40px; color: %1'>%2</p>")
                              .arg(palette().color(QPalette::Mid).name()).arg(tr("No transfers queued")), this))
{
    setWindowTitle(tr("Transfers"));
    setCentralWidget(new QWidget);
        
    m_view->setModel(m_model);
    m_view->setSelectionBehavior(QTreeView::SelectRows);
    m_view->setContextMenuPolicy(Qt::CustomContextMenu);
    m_view->setEditTriggers(QTreeView::NoEditTriggers);
    m_view->setItemsExpandable(false);
    m_view->setIndentation(0);
    m_view->setUniformRowHeights(true);
    m_view->setAllColumnsShowFocus(true);
    m_view->setColumnHidden(1, true);
    
    QHeaderView *header = m_view->header();
    QFontMetrics fm = header->fontMetrics();
    header->resizeSection(0, fm.width("A long transfer name") + 10);
    header->resizeSection(1, fm.width("A long category name") + 10);
    header->resizeSection(2, fm.width(m_model->headerData(2).toString()) + 10);
    header->resizeSection(3, fm.width("999.99MB of 999.99MB (99.99%)") + 10);
    header->setSectionHidden(1, true);
    
    m_transferConvertToAudioAction->setCheckable(true);
    m_transferCategoryGroup->setExclusive(true);
    m_transferPriorityGroup->setExclusive(true);
    m_transferHighPriorityAction->setCheckable(true);
    m_transferHighPriorityAction->setActionGroup(m_transferPriorityGroup);
    m_transferNormalPriorityAction->setCheckable(true);
    m_transferNormalPriorityAction->setActionGroup(m_transferPriorityGroup);
    m_transferLowPriorityAction->setCheckable(true);
    m_transferLowPriorityAction->setActionGroup(m_transferPriorityGroup);
    
    m_label->hide();
    
    m_layout = new QVBoxLayout(centralWidget());
    m_layout->addWidget(m_view);
    m_layout->addWidget(m_label);
    m_layout->setContentsMargins(0, 0, 0, 0);
    
    menuBar()->addAction(m_startAction);
    menuBar()->addAction(m_pauseAction);
    
    connect(m_model, SIGNAL(countChanged(int)), this, SLOT(onCountChanged(int)));
    connect(m_startAction, SIGNAL(triggered()), Transfers::instance(), SLOT(start()));
    connect(m_pauseAction, SIGNAL(triggered()), Transfers::instance(), SLOT(pause()));
    connect(m_view, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(showContextMenu(QPoint)));
    connect(m_contextMenu, SIGNAL(aboutToShow()), this, SLOT(setTransferMenuActions()));
    connect(Settings::instance(), SIGNAL(categoriesChanged()), this, SLOT(setCategoryMenuActions()));
    
    onCountChanged(m_model->rowCount());
    setCategoryMenuActions();
}

void TransfersWindow::onCountChanged(int count) {
    if (count > 0) {
        m_label->hide();
        m_view->show();
        m_startAction->setEnabled(true);
        m_pauseAction->setEnabled(true);
    }
    else {
        m_view->hide();
        m_label->show();
        m_startAction->setEnabled(false);
        m_pauseAction->setEnabled(false);
    }
}

void TransfersWindow::setCategoryMenuActions() {
    m_transferCategoryMenu->clear();

    QStringList categories = Settings::instance()->categoryNames();

    m_transferCategoryMenu->setEnabled(!categories.isEmpty());

    foreach (QString category, categories) {
        QAction *transferAction = m_transferCategoryMenu->addAction(category, this, SLOT(setCurrentTransferCategory()));
        transferAction->setCheckable(true);
        transferAction->setActionGroup(m_transferCategoryGroup);
    }
}

void TransfersWindow::setTransferMenuActions() {
    QModelIndex index = m_view->currentIndex();

    if (!index.isValid()) {
        return;
    }

    m_transferConvertToAudioAction->setEnabled(index.data(TransferModel::CanConvertToAudioRole).toBool());
    m_transferConvertToAudioAction->setChecked((m_transferConvertToAudioAction->isEnabled())
                                                && (index.data(TransferModel::ConvertToAudioRole).toBool()));

    switch (index.data(TransferModel::StatusRole).toInt()) {
    case Transfer::Paused:
    case Transfer::Failed:
        m_transferStartAction->setEnabled(true);
        m_transferPauseAction->setEnabled(false);
        break;
    default:
        m_transferStartAction->setEnabled(false);
        m_transferPauseAction->setEnabled(true);
    }

    switch (index.data(TransferModel::PriorityRole).toInt()) {
    case Transfer::HighPriority:
        m_transferHighPriorityAction->setChecked(true);
        break;
    case Transfer::LowPriority:
        m_transferLowPriorityAction->setChecked(true);
        break;
    default:
        m_transferNormalPriorityAction->setChecked(true);
    }

    QString category = index.data(TransferModel::CategoryRole).toString();

    foreach (QAction *action, m_transferCategoryMenu->actions()) {
        if (action->text() == category) {
            action->setChecked(true);
            return;
        }
    }
    
    if (!m_transferCategoryMenu->actions().isEmpty()) {
        m_transferCategoryMenu->actions().first()->setChecked(true);
    }
}

void TransfersWindow::showContextMenu(const QPoint &pos) {
    m_contextMenu->popup(m_view->mapToGlobal(pos), m_transferConvertToAudioAction);
}

void TransfersWindow::startCurrentTransfer() {
    if (m_view->currentIndex().isValid()) {
        if (Transfer *transfer = Transfers::instance()->get(m_view->currentIndex().row())) {
            transfer->queue();
        }
    }
}

void TransfersWindow::pauseCurrentTransfer() {
    if (m_view->currentIndex().isValid()) {
        if (Transfer *transfer = Transfers::instance()->get(m_view->currentIndex().row())) {
            transfer->pause();
        }
    }
}

void TransfersWindow::removeCurrentTransfer() {
    if (m_view->currentIndex().isValid()) {
        if (Transfer *transfer = Transfers::instance()->get(m_view->currentIndex().row())) {
            transfer->cancel();
        }
    }
}

void TransfersWindow::setConvertCurrentTransferToAudio() {
    if (m_view->currentIndex().isValid()) {
        m_model->setData(m_view->currentIndex(), m_transferConvertToAudioAction->isChecked(),
                         TransferModel::ConvertToAudioRole);
    }
}

void TransfersWindow::setCurrentTransferCategory() {
    if (m_view->currentIndex().isValid()) {
        if (QAction *action = qobject_cast<QAction*>(sender())) {
            m_model->setData(m_view->currentIndex(), action->text(), TransferModel::CategoryRole);
        }
    }
}

void TransfersWindow::setCurrentTransferPriority() {
    if (m_view->currentIndex().isValid()) {
        if (m_transferPriorityGroup->checkedAction() == m_transferHighPriorityAction) {
            m_model->setData(m_view->currentIndex(), Transfer::HighPriority, TransferModel::PriorityRole);
        }
        else if (m_transferPriorityGroup->checkedAction() == m_transferLowPriorityAction) {
            m_model->setData(m_view->currentIndex(), Transfer::LowPriority, TransferModel::PriorityRole);
        }
        else {
            m_model->setData(m_view->currentIndex(), Transfer::NormalPriority, TransferModel::PriorityRole);
        }
    }
}
