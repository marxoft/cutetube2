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

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "page.h"

class PluginManager;
class QActionGroup;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);

private Q_SLOTS:
    void loadPlugins();
    
    void setCurrentService();
    void setCurrentService(const QString &service);
    
    void showAboutDialog();
    void showSettingsDialog();
    void showTransfers();
    void showVideoPlayer();    

    void onPageStatusChanged(Page::Status status);
    void onPageWindowTitleChanged(const QString &text);
    
    void onStackCountChanged(int count);
    void onStackPageChanged(Page *page);
    
private:
    virtual void closeEvent(QCloseEvent *event);
    
    PluginManager *m_pluginManager;
    
    QMenu *m_serviceMenu;
    QMenu *m_viewMenu;
    QMenu *m_editMenu;
    QMenu *m_helpMenu;

    QActionGroup *m_serviceGroup;

    QAction *m_pluginsAction;
    QAction *m_quitAction;

    QAction *m_backAction;
    QAction *m_reloadAction;
    QAction *m_transfersAction;
    QAction *m_playerAction;

    QAction *m_settingsAction;

    QAction *m_aboutAction;

    QToolBar *m_toolBar;
};

#endif // MAINWINDOW_H
