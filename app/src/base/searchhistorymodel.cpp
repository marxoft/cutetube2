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

#include "searchhistorymodel.h"
#include "settings.h"
#include <QStringListModel>

SearchHistoryModel::SearchHistoryModel(QObject *parent) :
    QSortFilterProxyModel(parent),
    m_model(new QStringListModel(Settings::searchHistory(), this)),
    m_alignment(Qt::AlignCenter)
{
    setSourceModel(m_model);
    setFilterCaseSensitivity(Qt::CaseInsensitive);
    setDynamicSortFilter(true);

    connect(Settings::instance(), SIGNAL(searchHistoryChanged()), this, SLOT(reload()));
}

Qt::Alignment SearchHistoryModel::textAlignment() const {
    return m_alignment;
}

void SearchHistoryModel::setTextAlignment(Qt::Alignment align) {
    if (align != textAlignment()) {
        m_alignment = align;
        emit textAlignmentChanged();

        if (rowCount() > 0) {
            emit dataChanged(index(0, 0), index(rowCount() - 1, 0));
        }
    }
}

QVariant SearchHistoryModel::data(const QModelIndex &index, int role) const {
    switch (role) {
    case Qt::TextAlignmentRole:
        return QVariant(textAlignment());
    default:
        return QSortFilterProxyModel::data(index, role);
    }
}

QVariant SearchHistoryModel::data(int row, const QByteArray &role) const {
    return data(index(row, 0), roleNames().key(role));
}

void SearchHistoryModel::addSearch(const QString &query) {
    Settings::addSearch(query);
}

void SearchHistoryModel::removeSearch(const QString &query) {
    Settings::removeSearch(query);
}

void SearchHistoryModel::removeSearch(int row) {
    Settings::removeSearch(data(index(row, 0)).toString());
}

void SearchHistoryModel::clear() {
    Settings::setSearchHistory(QStringList());
}

void SearchHistoryModel::reload() {
    m_model->setStringList(Settings::searchHistory());
    emit countChanged(rowCount());
}
