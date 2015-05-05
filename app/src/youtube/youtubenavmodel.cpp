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

#include "youtubenavmodel.h"
#include "youtube.h"

YouTubeNavModel::YouTubeNavModel(QObject *parent) :
    QStringListModel(parent)
{
    reload();
    connect(YouTube::instance(), SIGNAL(userIdChanged()), this, SLOT(reload()));
}

void YouTubeNavModel::reload() {
#ifdef NAV_SEARCH
    if (YouTube::instance()->userId().isEmpty()) {
        setStringList(QStringList() << tr("Accounts") << tr("Search"));
    }
    else {
        setStringList(QStringList() << tr("Accounts") << tr("Search") << tr("My videos") << tr("Favourites")
                                    << tr("Likes") << tr("Watch later") << tr("Watch history") << tr("Playlists")
                                    << tr("Subscriptions"));
    }
#else
    if (YouTube::instance()->userId().isEmpty()) {
        setStringList(QStringList() << tr("Accounts"));
    }
    else {
        setStringList(QStringList() << tr("Accounts") << tr("My videos") << tr("Favourites") << tr("Likes")
                                    << tr("Watch later") << tr("Watch history") << tr("Playlists")
                                    << tr("Subscriptions"));
    }
#endif
}
