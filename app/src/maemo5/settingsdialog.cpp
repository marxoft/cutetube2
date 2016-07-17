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
#include "categoriesdialog.h"
#include "concurrenttransfersmodel.h"
#include "listview.h"
#include "localemodel.h"
#include "networkproxydialog.h"
#include "pluginconfigmodel.h"
#include "pluginsettingsdialog.h"
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
#include <QMaemo5InformationBox>

SettingsDialog::SettingsDialog(QWidget *parent) :
    Dialog(parent),
    m_transfersModel(new ConcurrentTransfersModel(this)),
    m_localeModel(new LocaleModel(this)),
    m_playerModel(new VideoPlayerModel(this)),
    m_pluginModel(new PluginConfigModel(this)),
    m_localeSelector(new ValueSelector(tr("Language filter"), this)),
    m_playerSelector(new ValueSelector(tr("Video player"), this)),
    m_transfersSelector(new ValueSelector(tr("Maximum concurrent transfers"), this)),
    m_pluginView(new ListView(this)),
    m_scrollArea(new QScrollArea(this)),
    m_downloadPathSelector(new QMaemo5ValueButton(tr("Default download path"), this)),
    m_playerCommandEdit(new QLineEdit(this)),
    m_transferCommandEdit(new QLineEdit(this)),
    m_clipboardCheckBox(new QCheckBox(tr("Monitor clipboard for URLs"), this)),
    m_safeSearchCheckBox(new QCheckBox(tr("Enable safe search"), this)),
    m_transfersCheckBox(new QCheckBox(tr("Start transfers automatically"), this)),
    m_transferCommandCheckBox(new QCheckBox(tr("Enable custom command"), this)),
    m_categoriesButton(new QPushButton(tr("Categories"), this)),
    m_proxyButton(new QPushButton(tr("Network proxy"), this)),
    m_buttonBox(new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, Qt::Vertical, this)),
    m_layout(new QHBoxLayout(this))
{
    setWindowTitle(tr("Settings"));
    setMinimumHeight(360);
    
    m_downloadPathSelector->setValueText(Settings::downloadPath());
    m_localeSelector->setModel(m_localeModel);
    m_localeSelector->setValue(Settings::locale());
    m_playerSelector->setModel(m_playerModel);
    m_playerSelector->setValue(Settings::videoPlayer());
    m_transfersSelector->setModel(m_transfersModel);
    m_transfersSelector->setValue(Settings::maximumConcurrentTransfers());
    m_pluginView->setModel(m_pluginModel);
    m_pluginView->setFixedHeight(m_pluginModel->rowCount() > 0
                                 ? m_pluginModel->rowCount() * m_pluginView->sizeHintForRow(0) : 0);
    m_playerCommandEdit->setText(Settings::videoPlayerCommand());
    m_transferCommandEdit->setText(Settings::customTransferCommand());
    m_clipboardCheckBox->setChecked(Settings::clipboardMonitorEnabled());
    m_safeSearchCheckBox->setChecked(Settings::safeSearchEnabled());
    m_transfersCheckBox->setChecked(Settings::startTransfersAutomatically());
    m_transferCommandCheckBox->setChecked(Settings::customTransferCommandEnabled());
    
    QWidget *scrollWidget = new QWidget(m_scrollArea);
    QVBoxLayout *vbox = new QVBoxLayout(scrollWidget);
    vbox->addWidget(new QLabel(tr("Media/content"), this));
    vbox->addWidget(m_downloadPathSelector);
    vbox->addWidget(m_localeSelector);
    vbox->addWidget(m_playerSelector);
    vbox->addWidget(new QLabel(tr("Custom command (%u for URI)"), this));
    vbox->addWidget(m_playerCommandEdit);
    vbox->addWidget(m_safeSearchCheckBox);
    vbox->addWidget(m_clipboardCheckBox);
    vbox->addWidget(new QLabel(tr("Transfers"), this));
    vbox->addWidget(m_transfersCheckBox);
    vbox->addWidget(m_transfersSelector);
    vbox->addWidget(new QLabel(tr("Custom command (%f for filename)"), this)),
    vbox->addWidget(m_transferCommandEdit);
    vbox->addWidget(m_transferCommandCheckBox);
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
    connect(m_localeSelector, SIGNAL(valueChanged(QVariant)), this, SLOT(onLocaleChanged(QVariant)));
    connect(m_playerSelector, SIGNAL(valueChanged(QVariant)), this, SLOT(onVideoPlayerChanged(QVariant)));
    connect(m_playerCommandEdit, SIGNAL(textEdited(QString)), Settings::instance(), SLOT(setVideoPlayerCommand(QString)));
    connect(m_transferCommandEdit, SIGNAL(textEdited(QString)), Settings::instance(), SLOT(setCustomTransferCommand(QString)));
    connect(m_clipboardCheckBox, SIGNAL(toggled(bool)), Settings::instance(), SLOT(setClipboardMonitorEnabled(bool)));
    connect(m_safeSearchCheckBox, SIGNAL(toggled(bool)), Settings::instance(), SLOT(setSafeSearchEnabled(bool)));
    connect(m_transfersCheckBox, SIGNAL(toggled(bool)), Settings::instance(), SLOT(setStartTransfersAutomatically(bool)));
    connect(m_transferCommandCheckBox, SIGNAL(toggled(bool)), Settings::instance(), SLOT(setCustomTransferCommandEnabled(bool)));
    connect(m_proxyButton, SIGNAL(clicked()), this, SLOT(showNetworkProxyDialog()));
    connect(m_categoriesButton, SIGNAL(clicked()), this, SLOT(showCategoriesDialog()));
    connect(m_pluginView, SIGNAL(activated(QModelIndex)), this, SLOT(showPluginDialog(QModelIndex)));
    connect(m_buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
    connect(m_buttonBox, SIGNAL(rejected()), this, SLOT(reject()));
}

void SettingsDialog::showCategoriesDialog() {
    CategoriesDialog(this).exec();
}

void SettingsDialog::showFileDialog() {
    const QString path = QFileDialog::getExistingDirectory(this, tr("Default download path"),
                                                           Settings::downloadPath());

    if (!path.isEmpty()) {
        m_downloadPathSelector->setValueText(path);
        Settings::setDownloadPath(path);
    }
}

void SettingsDialog::showNetworkProxyDialog() {
    NetworkProxyDialog(this).exec();
}

void SettingsDialog::showPluginDialog(const QModelIndex &index) {
    const QVariantList settings = index.data(PluginConfigModel::SettingsRole).toList();

    if (settings.isEmpty()) {
        QMaemo5InformationBox::information(this, tr("No settings for this plugin"));
    }
    else {
        PluginSettingsDialog dialog(index.data(PluginConfigModel::IdRole).toString(), settings, this);
        dialog.setWindowTitle(index.data(PluginConfigModel::DisplayNameRole).toString());
        dialog.exec();
    }
}

void SettingsDialog::onLocaleChanged(const QVariant &locale) {
    Settings::setLocale(locale.toString());
}

void SettingsDialog::onVideoPlayerChanged(const QVariant &player) {
    Settings::setVideoPlayer(player.toString());
}
