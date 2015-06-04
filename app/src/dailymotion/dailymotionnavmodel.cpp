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

#include "dailymotionnavmodel.h"
#include "dailymotion.h"

DailymotionNavModel::DailymotionNavModel(QObject *parent) :
    QStringListModel(parent)
{
    reload();
    connect(Dailymotion::instance(), SIGNAL(userIdChanged()), this, SLOT(reload()));
}

void DailymotionNavModel::reload() {
    if (Dailymotion::instance()->userId().isEmpty()) {
        setStringList(QStringList() << tr("Accounts") << tr("Search") << tr("Categories"));
    }
    else {
        setStringList(QStringList() << tr("Accounts") << tr("Search") << tr("Categories") << tr("My videos")
                                    << tr("Latest videos") << tr("Favourites") << tr("Playlists")
                                    << tr("Subscriptions"));
    }
}
