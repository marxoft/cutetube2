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

#include "mainwindow.h"
#include "dailymotionview.h"
#include "pluginmanager.h"
#include "pluginview.h"
#include "resources.h"
#include "settings.h"
#include "settingsdialog.h"
#include "transferswindow.h"
#include "videoplayerwindow.h"
#include "vimeoview.h"
#include "youtubeview.h"
#include <QActionGroup>
#include <QApplication>
#include <QMenu>
#include <QMenuBar>
#include <QMessageBox>
#include <QStatusBar>
#include <QToolBar>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    m_serviceMenu(new QMenu(tr("&Service"), this)),
    m_viewMenu(new QMenu(tr("&View"), this)),
    m_editMenu(new QMenu(tr("&Edit"), this)),
    m_helpMenu(new QMenu(tr("&About"), this)),
    m_serviceGroup(new QActionGroup(this)),
    m_pluginsAction(new QAction(QIcon::fromTheme("view-refresh"), tr("&Load plugins"), this)),
    m_quitAction(new QAction(QIcon::fromTheme("application-exit"), tr("&Quit"), this)),
    m_backAction(new QAction(QIcon::fromTheme("go-previous"), tr("Go &back"), this)),
    m_reloadAction(new QAction(QIcon::fromTheme("view-refresh"), tr("&Reload"), this)),
    m_transfersAction(new QAction(QIcon::fromTheme("go-down"), tr("Show &transfers"), this)),
    m_playerAction(new QAction(QIcon::fromTheme("media-playback-start"), tr("Show video &player"), this)),
    m_settingsAction(new QAction(QIcon::fromTheme("preferences-desktop"), tr("&Preferences"), this)),
    m_aboutAction(new QAction(QIcon::fromTheme("help-about"), tr("&About"), this)),
    m_toolBar(new QToolBar(this))
{
    setWindowTitle("cuteTube");
    addToolBar(Qt::TopToolBarArea, m_toolBar);

    statusBar();
    
    menuBar()->addMenu(m_serviceMenu);
    menuBar()->addMenu(m_viewMenu);
    menuBar()->addMenu(m_editMenu);
    menuBar()->addMenu(m_helpMenu);

    QAction *action = m_serviceMenu->addAction("YouTube", this, SLOT(setCurrentService()));
    action->setData(Resources::YOUTUBE);
    action->setCheckable(true);
    action->setShortcut(tr("Ctrl+1"));
    m_serviceGroup->addAction(action);

    action = m_serviceMenu->addAction("Dailymotion", this, SLOT(setCurrentService()));
    action->setData(Resources::DAILYMOTION);
    action->setCheckable(true);
    action->setShortcut(tr("Ctrl+2"));
    m_serviceGroup->addAction(action);

    action = m_serviceMenu->addAction("Vimeo", this, SLOT(setCurrentService()));
    action->setData(Resources::VIMEO);
    action->setCheckable(true);
    action->setShortcut(tr("Ctrl+3"));
    m_serviceGroup->addAction(action);

    int count = m_serviceGroup->actions().size();
    
    foreach (const ServicePluginPair &pair, PluginManager::instance()->plugins()) {
        action = m_serviceMenu->addAction(pair.config->displayName(), this, SLOT(setCurrentService()));
        action->setData(pair.config->id());
        action->setCheckable(true);
        action->setShortcut(tr("Ctrl+%1").arg(++count));
        m_serviceGroup->addAction(action);
    }

    m_serviceMenu->addSeparator();
    m_serviceMenu->addAction(m_pluginsAction);
    m_serviceMenu->addAction(m_quitAction);

    m_pluginsAction->setShortcut(tr("Ctrl+L"));
    m_quitAction->setShortcut(tr("Ctrl+Q"));

    m_viewMenu->addAction(m_backAction);
    m_viewMenu->addAction(m_reloadAction);
    m_viewMenu->addSeparator();
    m_viewMenu->addAction(m_transfersAction);
    m_viewMenu->addAction(m_playerAction);

    m_backAction->setShortcut(tr("Ctrl+B"));
    m_reloadAction->setShortcut(tr("Ctrl+R"));
    m_transfersAction->setShortcut(tr("Ctrl+T"));
    m_playerAction->setShortcut(tr("Ctrl+M"));

    m_editMenu->addAction(m_settingsAction);

    m_settingsAction->setShortcut(tr("Ctrl+P"));

    m_helpMenu->addAction(m_aboutAction);

    m_backAction->setEnabled(false);
    m_reloadAction->setEnabled(false);

    m_toolBar->setObjectName("mainToolBar");
    m_toolBar->setWindowTitle(tr("Main toolbar"));
    m_toolBar->setMovable(false);
    m_toolBar->setAllowedAreas(Qt::TopToolBarArea);
    m_toolBar->addAction(m_backAction);
    m_toolBar->addAction(m_reloadAction);
    m_toolBar->addSeparator();
    m_toolBar->addAction(m_transfersAction);
    m_toolBar->addAction(m_playerAction);

    connect(m_pluginsAction, SIGNAL(triggered()), this, SLOT(loadPlugins()));
    connect(m_quitAction, SIGNAL(triggered()), QApplication::instance(), SLOT(closeAllWindows()));
    connect(m_transfersAction, SIGNAL(triggered()), this, SLOT(showTransfers()));
    connect(m_playerAction, SIGNAL(triggered()), this, SLOT(showVideoPlayer()));
    connect(m_settingsAction, SIGNAL(triggered()), this, SLOT(showSettingsDialog()));
    connect(m_aboutAction, SIGNAL(triggered()), this, SLOT(showAboutDialog()));
    connect(Settings::instance(), SIGNAL(currentServiceChanged(QString)), this, SLOT(setCurrentService(QString)));

    setCurrentService(Settings::currentService());
    
    restoreGeometry(Settings::mainWindowGeometry());
    restoreState(Settings::mainWindowState());
}

void MainWindow::closeEvent(QCloseEvent *event) {
    Settings::setMainWindowGeometry(saveGeometry());
    Settings::setMainWindowState(saveState());
    QMainWindow::closeEvent(event);
}

void MainWindow::loadPlugins() {
    const int count = PluginManager::instance()->load();
    
    if (count > 0) {
        QMessageBox::information(this, tr("Load plugins"), tr("%1 new plugin(s) found"));
    }
    else {
        QMessageBox::information(this, tr("Load plugins"), tr("No new plugins found"));
    }
}

void MainWindow::setCurrentService() {
    if (const QAction *action = m_serviceGroup->checkedAction()) {
        Settings::setCurrentService(action->data().toString());
    }
}

void MainWindow::setCurrentService(const QString &service) {
    foreach(QAction *action, m_serviceGroup->actions()) {
        if (action->data() == service) {
            action->setChecked(true);
            break;
        }
    }
    
    m_backAction->setEnabled(false);
    m_reloadAction->setEnabled(false);

    PageStack *stack;
    
    if (service == Resources::YOUTUBE) {
        stack = new YouTubeView(this);
    }
    else if (service == Resources::DAILYMOTION) {
        stack = new DailymotionView(this);
    }
    else if (service == Resources::VIMEO) {
        stack = new VimeoView(this);
    }
    else {
        stack = new PluginView(service, this);
    }
    
    setCentralWidget(stack);
    setWindowTitle(stack->windowTitle());
    connect(stack, SIGNAL(countChanged(int)), this, SLOT(onStackCountChanged(int)));
    connect(stack, SIGNAL(currentChanged(Page*)), this, SLOT(onStackPageChanged(Page*)));
    connect(m_backAction, SIGNAL(triggered()), stack, SLOT(pop()));
}

void MainWindow::showAboutDialog() {}

void MainWindow::showSettingsDialog() {
    SettingsDialog(this).exec();
}

void MainWindow::showTransfers() {
    TransfersWindow::instance()->show();
    TransfersWindow::instance()->activateWindow();
}

void MainWindow::showVideoPlayer() {
    VideoPlayerWindow::instance()->show();
    VideoPlayerWindow::instance()->activateWindow();
}

void MainWindow::onPageStatusChanged(Page::Status status) {
    m_reloadAction->setEnabled(status != Page::Null);

    if (status == Page::Loading) {
        m_reloadAction->setIcon(QIcon::fromTheme("window-close"));
        m_reloadAction->setText(tr("&Cancel"));
    }
    else {
        m_reloadAction->setIcon(QIcon::fromTheme("view-refresh"));
        m_reloadAction->setText(tr("&Reload"));
    }
}

void MainWindow::onPageWindowTitleChanged(const QString &text) {
    setWindowTitle(centralWidget()->windowTitle() + " - " + text);
}

void MainWindow::onStackCountChanged(int count) {
    m_backAction->setEnabled(count > 1);
}

void MainWindow::onStackPageChanged(Page *page) {
    disconnect(m_reloadAction, SIGNAL(triggered()), 0, 0);
    
    if (!page) {
        onPageStatusChanged(Page::Null);
        return;
    }

    setWindowTitle(centralWidget()->windowTitle() + " - " + page->windowTitle());
    statusBar()->showMessage(page->statusText());
    onPageStatusChanged(page->status());
    connect(page, SIGNAL(statusChanged(Page::Status)), this, SLOT(onPageStatusChanged(Page::Status)));
    connect(page, SIGNAL(statusTextChanged(QString)), statusBar(), SLOT(showMessage(QString)));
    connect(page, SIGNAL(windowTitleChanged(QString)), this, SLOT(onPageWindowTitleChanged(QString)));
    connect(m_reloadAction, SIGNAL(triggered()), page, SLOT(toggleLoading()));
}
