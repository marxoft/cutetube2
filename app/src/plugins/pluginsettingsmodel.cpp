/*
 * Copyright (C) 2014 Stuart Howarth <showarth@marxoft.co.uk>
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms and conditions of the GNU Lesser General Public License,
 * version 3, as published by the Free Software Foundation.
 *
 * This program is distributed in the hope it will be useful, but WITHOUT ANY
 * WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for
 * more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program; if not, write to the Free Software Foundation,
 * Inc., 51 Franklin St - Fifth Floor, Boston, MA 02110-1301 USA.
 */

#include "pluginsettingsmodel.h"
#include "definitions.h"
#include <QDomDocument>
#include <QDomElement>
#include <QDir>
#include <QFile>
#include <QCoreApplication>
#include <QPixmap>

PluginSettingsModel::PluginSettingsModel(QObject *parent) :
    QAbstractListModel(parent)
{
#if QT_VERSION >= 0x040600
    m_roleNames[PluginNameRole] = "pluginName";
    m_roleNames[PluginIconRole] = "pluginIcon";
    m_roleNames[FileNameRole] = "fileName";
#if QT_VERSION < 0x050000
    this->setRoleNames(m_roleNames);
#endif
#endif
    this->loadSettingsFiles();
}

PluginSettingsModel::~PluginSettingsModel() {}

#if QT_VERSION >= 0x050000
QHash<int, QByteArray> PluginSettingsModel::roleNames() const {
    return m_roleNames;
}
#endif

int PluginSettingsModel::rowCount(const QModelIndex &parent) const {
    Q_UNUSED(parent)

    return m_list.size();
}

QVariant PluginSettingsModel::data(const QModelIndex &index, int role) const {
    if (!index.isValid()) {
        return QVariant();
    }

    switch (role) {
    case Qt::DisplayRole:
    case PluginNameRole:
        return m_list.at(index.row()).name;
    case Qt::DecorationRole:
        return QPixmap(m_list.at(index.row()).icon).scaled(ICON_SIZE, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
    case PluginIconRole:
        return m_list.at(index.row()).icon;
    case FileNameRole:
        return m_list.at(index.row()).fileName;
    default:
        return QVariant();
    }
}

#if QT_VERSION >= 0x040600
QVariant PluginSettingsModel::data(int row, const QByteArray &role) const {
    return this->data(this->index(row), this->roleNames().key(role));
}
#endif

void PluginSettingsModel::loadSettingsFiles() {
    if (this->rowCount() > 0) {
        this->beginRemoveRows(QModelIndex(), 0, this->rowCount() - 1);
        m_list.clear();
        this->endRemoveRows();
    }

    QDir dir(QCoreApplication::applicationDirPath());
    dir.cdUp();
    dir.cd("settings");

    foreach (QString fileName, dir.entryList(QStringList() << "*.xml", QDir::Files)) {
        QFile file(dir.absoluteFilePath(fileName));

        if (file.open(QIODevice::ReadOnly)) {
            QString xml(file.readAll());
            QDomDocument doc;
            doc.setContent(xml);
            QDomElement root = doc.documentElement();
            Setting setting;
            setting.name = root.attribute("title");
            setting.icon = ICON_PATH + root.attribute("icon");
            setting.fileName = file.fileName();

            this->beginInsertRows(QModelIndex(), this->rowCount(), this->rowCount());
            m_list.append(setting);
            this->endInsertRows();
        }
    }

    emit countChanged(this->rowCount());
}
