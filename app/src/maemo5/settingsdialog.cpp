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

#include "settingsdialog.h"
#include "categoriesdialog.h"
#include "concurrenttransfersmodel.h"
#include "listview.h"
#include "networkproxydialog.h"
#include "pluginsettingsdialog.h"
#include "pluginsettingsmodel.h"
#include "settings.h"
#include "valueselector.h"
#include "videoplayermodel.h"
#include <QLabel>
#include <QLineEdit>
#include <QCheckBox>
#include <QPushButton>
#include <QScrollArea>
#include <QDialogButtonBox>
#include <QHBoxLayout>
#include <QFileDialog>

SettingsDialog::SettingsDialog(QWidget *parent) :
    Dialog(parent),
    m_transfersModel(new ConcurrentTransfersModel(this)),
    m_playerModel(new VideoPlayerModel(this)),
    m_pluginModel(new PluginSettingsModel(this)),
    m_playerSelector(new ValueSelector(tr("Video player"), this)),
    m_transfersSelector(new ValueSelector(tr("Maximum concurrent transfers"), this)),
    m_pluginView(new ListView(this)),
    m_scrollArea(new QScrollArea(this)),
    m_downloadPathSelector(new QMaemo5ValueButton(tr("Default download path"), this)),
    m_commandEdit(new QLineEdit(this)),
    m_clipboardCheckBox(new QCheckBox(tr("Monitor clipboard for URLs"), this)),
    m_safeSearchCheckBox(new QCheckBox(tr("Enable safe search"), this)),
    m_transfersCheckBox(new QCheckBox(tr("Start transfers automatically"), this)),
    m_categoriesButton(new QPushButton(tr("Categories"), this)),
    m_proxyButton(new QPushButton(tr("Network proxy"), this)),
    m_buttonBox(new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, Qt::Vertical, this)),
    m_layout(new QHBoxLayout(this))
{
    setWindowTitle(tr("Settings"));
    
    m_downloadPathSelector->setValueText(Settings::instance()->downloadPath());
    m_playerSelector->setModel(m_playerModel);
    m_playerSelector->setValue(Settings::instance()->videoPlayer());
    m_transfersSelector->setModel(m_transfersModel);
    m_transfersSelector->setValue(Settings::instance()->maximumConcurrentTransfers());
    m_pluginView->setModel(m_pluginModel);
    m_pluginView->setFixedHeight(m_pluginModel->rowCount() > 0
                                 ? m_pluginModel->rowCount() * m_pluginView->sizeHintForRow(0) : 0);
    m_commandEdit->setPlaceholderText(tr("Custom command"));
    m_commandEdit->setText(Settings::instance()->videoPlayerCommand());
    m_commandEdit->setEnabled(Settings::instance()->videoPlayer() == "other");
    m_clipboardCheckBox->setChecked(Settings::instance()->clipboardMonitorEnabled());
    m_safeSearchCheckBox->setChecked(Settings::instance()->safeSearchEnabled());
    m_transfersCheckBox->setChecked(Settings::instance()->startTransfersAutomatically());
    
    QWidget *scrollWidget = new QWidget(m_scrollArea);
    QVBoxLayout *vbox = new QVBoxLayout(scrollWidget);
    vbox->addWidget(new QLabel(tr("Media/content"), this));
    vbox->addWidget(m_downloadPathSelector);
    vbox->addWidget(m_playerSelector);
    vbox->addWidget(m_commandEdit);
    vbox->addWidget(m_safeSearchCheckBox);
    vbox->addWidget(m_clipboardCheckBox);
    vbox->addWidget(new QLabel(tr("Transfers"), this));
    vbox->addWidget(m_transfersCheckBox);
    vbox->addWidget(m_transfersSelector);
    vbox->addWidget(m_proxyButton);
    vbox->addWidget(m_categoriesButton);
    vbox->addWidget(new QLabel(tr("Plugins"), this));
    vbox->addWidget(m_pluginView);
    vbox->setContentsMargins(0, 0, 0, 0);
    m_scrollArea->setWidget(scrollWidget);
    m_scrollArea->setWidgetResizable(true);
    
    m_layout->addWidget(m_scrollArea);
    m_layout->addWidget(m_buttonBox);
    
    connect(m_downloadPathSelector, SIGNAL(clicked()), this, SLOT(showFileDialog()));
    connect(m_playerSelector, SIGNAL(valueChanged(QVariant)), this, SLOT(onVideoPlayerChanged(QVariant)));
    connect(m_commandEdit, SIGNAL(textEdited(QString)), Settings::instance(), SLOT(setVideoPlayerCommand(QString)));
    connect(m_clipboardCheckBox, SIGNAL(toggled(bool)), Settings::instance(), SLOT(setClipboardMonitorEnabled(bool)));
    connect(m_safeSearchCheckBox, SIGNAL(toggled(bool)), Settings::instance(), SLOT(setSafeSearchEnabled(bool)));
    connect(m_transfersCheckBox, SIGNAL(toggled(bool)), Settings::instance(), SLOT(setStartTransfersAutomatically(bool)));
    connect(m_proxyButton, SIGNAL(clicked()), this, SLOT(showNetworkProxyDialog()));
    connect(m_categoriesButton, SIGNAL(clicked()), this, SLOT(showCategoriesDialog()));
    connect(m_pluginView, SIGNAL(activated(QModelIndex)), this, SLOT(showPluginDialog(QModelIndex)));
    connect(m_buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
    connect(m_buttonBox, SIGNAL(rejected()), this, SLOT(reject()));
}

void SettingsDialog::showCategoriesDialog() {
    CategoriesDialog *dialog = new CategoriesDialog(this);
    dialog->open();
}

void SettingsDialog::showFileDialog() {
    QString path = QFileDialog::getExistingDirectory(this, tr("Default download path"),
                                                     Settings::instance()->downloadPath());

    if (!path.isEmpty()) {
        m_downloadPathSelector->setValueText(path);
        Settings::instance()->setDownloadPath(path);
    }
}

void SettingsDialog::showNetworkProxyDialog() {
    NetworkProxyDialog *dialog = new NetworkProxyDialog(this);
    dialog->open();
}

void SettingsDialog::showPluginDialog(const QModelIndex &index) {
    PluginSettingsDialog *dialog = new PluginSettingsDialog(index.data(PluginSettingsModel::NameRole).toString(),
                                                            index.data(PluginSettingsModel::ValueRole).toString(),
                                                            this);
    dialog->open();
}

void SettingsDialog::onVideoPlayerChanged(const QVariant &player) {
    m_commandEdit->setEnabled(player == "other");
    Settings::instance()->setVideoPlayer(player.toString());
}
