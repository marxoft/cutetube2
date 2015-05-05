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

#ifndef PLAYLISTTHUMBNAIL_H
#define PLAYLISTTHUMBNAIL_H

#include "image.h"

class PlaylistThumbnail : public Image
{
    Q_OBJECT
    
    Q_PROPERTY(int videoCount READ videoCount WRITE setVideoCount)
    
public:
    explicit PlaylistThumbnail(QWidget *parent = 0);
    
    int videoCount() const;
    void setVideoCount(int count);
    
protected:
    void paintEvent(QPaintEvent *e);
    
    int m_count;
};
    
#endif // PLAYLISTTHUMBNAIL_H
