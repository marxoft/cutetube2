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

#ifndef VIDEOTHUMBNAIL_H
#define VIDEOTHUMBNAIL_H

#include "image.h"

class VideoThumbnail : public Image
{
    Q_OBJECT
    
    Q_PROPERTY(QString duration READ duration WRITE setDuration)
    
public:
    explicit VideoThumbnail(QWidget *parent = 0);
    
    QString duration() const;
    void setDuration(const QString &d);
    
protected:
    void paintEvent(QPaintEvent *e);
    
    QString m_duration;
};
    
#endif // VIDEOTHUMBNAIL_H
