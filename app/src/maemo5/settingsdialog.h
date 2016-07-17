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

#include "dialog.h"

class ConcurrentTransfersModel;
class LocaleModel;
class VideoPlayerModel;
class PluginConfigModel;
class ListView;
class ValueSelector;
class QLineEdit;
class QPushButton;
class QCheckBox;
class QMaemo5ValueButton;
class QDialogButtonBox;
class QHBoxLayout;
class QModelIndex;
class QScrollArea;

class SettingsDialog : public Dialog
{
    Q_OBJECT
    
public:
    explicit SettingsDialog(QWidget *parent = 0);
    
private Q_SLOTS:
    void showCategoriesDialog();
    void showFileDialog();
    void showNetworkProxyDialog();
    void showPluginDialog(const QModelIndex &index);
    
    void onLocaleChanged(const QVariant &locale);
    void onVideoPlayerChanged(const QVariant &player);
    
private:
    ConcurrentTransfersModel *m_transfersModel;
    LocaleModel *m_localeModel;
    VideoPlayerModel *m_playerModel;
    PluginConfigModel *m_pluginModel;
    
    ValueSelector *m_localeSelector;
    ValueSelector *m_playerSelector;
    ValueSelector *m_transfersSelector;
    ListView *m_pluginView;
    QScrollArea *m_scrollArea;
    QMaemo5ValueButton *m_downloadPathSelector;
    QLineEdit *m_playerCommandEdit;
    QLineEdit *m_transferCommandEdit;
    QCheckBox *m_clipboardCheckBox;
    QCheckBox *m_safeSearchCheckBox;
    QCheckBox *m_transfersCheckBox;
    QCheckBox *m_transferCommandCheckBox;
    QPushButton *m_categoriesButton;
    QPushButton *m_proxyButton;
    QDialogButtonBox *m_buttonBox;
    QHBoxLayout *m_layout;
};

#endif // SETTINGSDIALOG_H
