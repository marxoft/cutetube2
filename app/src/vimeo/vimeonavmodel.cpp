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

#include "vimeonavmodel.h"
#include "vimeo.h"

VimeoNavModel::VimeoNavModel(QObject *parent) :
    QStringListModel(parent)
{
    reload();
    connect(Vimeo::instance(), SIGNAL(userIdChanged(QString)), this, SLOT(reload()));
}

void VimeoNavModel::reload() {
    if (Vimeo::userId().isEmpty()) {
        setStringList(QStringList() << tr("Accounts") << tr("Search") << tr("Categories"));
    }
    else {
        setStringList(QStringList() << tr("Accounts") << tr("Search") << tr("Categories") << tr("My videos")
                                    << tr("Latest videos") << tr("Likes") << tr("Watch later") << tr("Albums")
                                    << tr("Subscriptions"));
    }
}
