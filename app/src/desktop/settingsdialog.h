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

#ifndef SETTINGSDIALOG_H
#define SETTINGSDIALOG_H

#include <QDialog>

class CategorySettingsTab;
class GeneralSettingsTab;
class NetworkSettingsTab;
class PluginsSettingsTab;
class TransferSettingsTab;
class QDialogButtonBox;
class QStackedWidget;
class QTabBar;
class QVBoxLayout;

class SettingsDialog : public QDialog
{
    Q_OBJECT

public:
    explicit SettingsDialog(QWidget *parent = 0);

public Q_SLOTS:
    virtual void accept();

private Q_SLOTS:
    void setCurrentTab(int index);
    
    void showGeneralTab();
    void showTransferTab();
    void showNetworkTab();
    void showCategoryTab();
    void showPluginsTab();

private:
    GeneralSettingsTab *m_generalTab;
    TransferSettingsTab *m_transferTab;
    NetworkSettingsTab *m_networkTab;
    CategorySettingsTab *m_categoryTab;
    PluginsSettingsTab *m_pluginsTab;
    
    QTabBar *m_tabBar;

    QStackedWidget *m_stack;

    QDialogButtonBox *m_buttonBox;

    QVBoxLayout *m_layout;
};

#endif // SETTINGSDIALOG_H
