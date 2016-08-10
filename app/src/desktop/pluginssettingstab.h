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

#ifndef PLUGINSSETTINGSTAB_H
#define PLUGINSSETTINGSTAB_H

#include "settingstab.h"

class PluginConfigModel;
class QListView;
class QScrollArea;
class QSplitter;
class QVBoxLayout;

class PluginsSettingsTab : public SettingsTab
{
    Q_OBJECT

public:
    explicit PluginsSettingsTab(QWidget *parent = 0);

public Q_SLOTS:
    virtual void save();

private Q_SLOTS:
    void setCurrentPlugin(const QModelIndex &index);

private:
    PluginConfigModel *m_model;

    QListView *m_view;

    QScrollArea *m_scrollArea;

    QSplitter *m_splitter;

    QVBoxLayout *m_layout;
};

#endif // PLUGINSSETTINGSTAB_H
