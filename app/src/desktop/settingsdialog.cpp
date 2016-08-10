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

#include "settingsdialog.h"
#include "categorysettingstab.h"
#include "generalsettingstab.h"
#include "networksettingstab.h"
#include "pluginssettingstab.h"
#include "transfersettingstab.h"
#include <QDialogButtonBox>
#include <QPushButton>
#include <QStackedWidget>
#include <QTabBar>
#include <QVBoxLayout>

SettingsDialog::SettingsDialog(QWidget *parent) :
    QDialog(parent),
    m_generalTab(0),
    m_transferTab(0),
    m_networkTab(0),
    m_categoryTab(0),
    m_pluginsTab(0),
    m_tabBar(new QTabBar(this)),
    m_stack(new QStackedWidget(this)),
    m_buttonBox(new QDialogButtonBox(QDialogButtonBox::Cancel | QDialogButtonBox::Save, Qt::Horizontal, this)),
    m_layout(new QVBoxLayout(this))
{
    setWindowTitle(tr("Preferences"));

    m_tabBar->addTab(tr("General"));
    m_tabBar->addTab(tr("Transfers"));
    m_tabBar->addTab(tr("Network"));
    m_tabBar->addTab(tr("Categories"));
    m_tabBar->addTab(tr("Plugins"));

    m_buttonBox->button(QDialogButtonBox::Cancel)->setDefault(false);

    m_layout->addWidget(m_tabBar);
    m_layout->addWidget(m_stack);
    m_layout->addWidget(m_buttonBox);

    connect(m_tabBar, SIGNAL(currentChanged(int)), this, SLOT(setCurrentTab(int)));
    connect(m_buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
    connect(m_buttonBox, SIGNAL(rejected()), this, SLOT(reject()));

    showGeneralTab();
}

void SettingsDialog::accept() {
    for (int i = 0; i < m_stack->count(); i++) {
        if (SettingsTab *tab = qobject_cast<SettingsTab*>(m_stack->widget(i))) {
            tab->save();
        }
    }

    QDialog::accept();
}

void SettingsDialog::setCurrentTab(int index) {
    switch (index) {
    case 0:
        showGeneralTab();
        break;
    case 1:
        showTransferTab();
        break;
    case 2:
        showNetworkTab();
        break;
    case 3:
        showCategoryTab();
        break;
    case 4:
        showPluginsTab();
        break;
    default:
        break;
    }
}

void SettingsDialog::showGeneralTab() {
    if (!m_generalTab) {
        m_generalTab = new GeneralSettingsTab(m_stack);
        m_stack->addWidget(m_generalTab);
    }

    m_stack->setCurrentWidget(m_generalTab);
}

void SettingsDialog::showTransferTab() {
    if (!m_transferTab) {
        m_transferTab = new TransferSettingsTab(m_stack);
        m_stack->addWidget(m_transferTab);
    }

    m_stack->setCurrentWidget(m_transferTab);
}

void SettingsDialog::showNetworkTab() {
    if (!m_networkTab) {
        m_networkTab = new NetworkSettingsTab(m_stack);
        m_stack->addWidget(m_networkTab);
    }

    m_stack->setCurrentWidget(m_networkTab);
}

void SettingsDialog::showCategoryTab() {
    if (!m_categoryTab) {
        m_categoryTab = new CategorySettingsTab(m_stack);
        m_stack->addWidget(m_categoryTab);
    }

    m_stack->setCurrentWidget(m_categoryTab);
}

void SettingsDialog::showPluginsTab() {
    if (!m_pluginsTab) {
        m_pluginsTab = new PluginsSettingsTab(m_stack);
        m_stack->addWidget(m_pluginsTab);
    }

    m_stack->setCurrentWidget(m_pluginsTab);
}
