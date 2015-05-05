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

#include "vimeonavmodel.h"
#include "vimeo.h"

VimeoNavModel::VimeoNavModel(QObject *parent) :
    QStringListModel(parent)
{
    reload();
    connect(Vimeo::instance(), SIGNAL(userIdChanged()), this, SLOT(reload()));
}

void VimeoNavModel::reload() {
#ifdef NAV_SEARCH
    if (Vimeo::instance()->userId().isEmpty()) {
        setStringList(QStringList() << tr("Accounts") << tr("Search"));
    }
    else {
        setStringList(QStringList() << tr("Accounts") << tr("Search") << tr("My videos") << tr("Latest videos")
                                    << tr("Likes") << tr("Watch later") << tr("Albums") << tr("Subscriptions"));
    }
#else
    if (Vimeo::instance()->userId().isEmpty()) {
        setStringList(QStringList() << tr("Accounts"));
    }
    else {
        setStringList(QStringList() << tr("Accounts") << tr("My videos") << tr("Latest videos") << tr("Likes")
                                    << tr("Watch later") << tr("Albums") << tr("Subscriptions"));
    }
#endif
}
