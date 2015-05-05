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

#ifndef PLUGINPLAYLISTDELEGATE_H
#define PLUGINPLAYLISTDELEGATE_H

#include <QStyledItemDelegate>

class ImageCache;

class PluginPlaylistDelegate : public QStyledItemDelegate
{
    Q_OBJECT
    
public:
    explicit PluginPlaylistDelegate(ImageCache *cache, QObject *parent = 0);
    
    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const;
    
    QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &) const;
    
    bool gridMode() const;
    void setGridMode(bool enabled);
    
private:
    ImageCache *m_cache;
    
    bool m_gridMode;
};

#endif // PLUGINPLAYLISTDELEGATE_H
