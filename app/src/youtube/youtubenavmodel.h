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

#ifndef YOUTUBENAVMODEL_H
#define YOUTUBENAVMODEL_H

#include <QStringListModel>

class YouTubeNavModel : public QStringListModel
{
    Q_OBJECT

public:
    explicit YouTubeNavModel(QObject *parent = 0);
    
public Q_SLOTS:
    void reload();
};

#endif // YOUTUBENAVMODEL_H
