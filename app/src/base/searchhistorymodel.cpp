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

#include "searchhistorymodel.h"
#include "settings.h"
#include <QStringListModel>

SearchHistoryModel::SearchHistoryModel(QObject *parent) :
    QSortFilterProxyModel(parent),
    m_model(new QStringListModel(Settings::instance()->searchHistory(), this))
{
    setFilterCaseSensitivity(Qt::CaseInsensitive);
    setSourceModel(m_model);
    
    m_roles[Qt::DisplayRole] = "display";
    m_roles[OriginalStringRole] = "originalString";
#if QT_VERSION < 0x050000
    setRoleNames(m_roles);
#endif
    connect(Settings::instance(), SIGNAL(searchHistoryChanged()), this, SLOT(reload()));
}

#if QT_VERSION >= 0x050000
QHash<int, QByteArray> SearchHistoryModel::roleNames() const {
    return m_roles;
}
#endif

QVariant SearchHistoryModel::data(const QModelIndex &index, int role) const {
    switch (role) {
    case Qt::DisplayRole: {        
        if (!filterRegExp().isEmpty()) {
            QRegExp re = filterRegExp();
            QString os = data(index, OriginalStringRole).toString();
        
            if (re.indexIn(os) != -1) {
                return os.replace(re, QString("<b>%1</b>").arg(re.cap()));
            }
        }
        
        break;
    }
    case Qt::TextAlignmentRole:
        return Qt::AlignCenter;
    case OriginalStringRole:
        return QSortFilterProxyModel::data(index, Qt::DisplayRole);
    default:
        break;
    }
    
    return QSortFilterProxyModel::data(index, role);
}

void SearchHistoryModel::addSearch(const QString &query) {
    Settings::instance()->addSearch(query);
}

void SearchHistoryModel::removeSearch(const QString &query) {
    Settings::instance()->removeSearch(query);
}

void SearchHistoryModel::removeSearch(int row) {
    Settings::instance()->removeSearch(data(index(row, 0, QModelIndex()), OriginalStringRole).toString());
}

void SearchHistoryModel::clear() {
    Settings::instance()->setSearchHistory(QStringList());
}

void SearchHistoryModel::reload() {
    m_model->setStringList(Settings::instance()->searchHistory());
    emit countChanged(rowCount());
}
