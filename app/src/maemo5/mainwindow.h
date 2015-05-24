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

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "stackedwindow.h"
#include <QVariantMap>

class Transfer;
class ServiceModel;
class ValueSelectorAction;
class QAction;
class QLabel;

class MainWindow : public StackedWindow
{
    Q_OBJECT

public:
    explicit MainWindow(StackedWindow *parent = 0);
    ~MainWindow();
    
    static MainWindow* instance();
    
public Q_SLOTS:
    bool search(const QString &service, const QString &query, const QString &type, const QString &order);
    bool showResource(const QString &url);
    bool showResource(const QVariantMap &resource);
    
private Q_SLOTS:
    void setService(const QVariant &service);
    
    void showAboutDialog();
    void showSettingsDialog();
    void showTransfers();
    
    void onTransferAdded(Transfer *transfer);
        
private:
    static MainWindow *self;
    
    ServiceModel *m_serviceModel;
    
    ValueSelectorAction *m_serviceAction;
    QAction *m_transfersAction;
    QAction *m_settingsAction;
    QAction *m_aboutAction;
};

#endif // MAINWINDOW_H
