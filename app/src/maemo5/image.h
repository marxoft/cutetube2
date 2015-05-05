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

#ifndef IMAGE_H
#define IMAGE_H

#include <QWidget>
#include <QUrl>

class ImageCache;

class Image : public QWidget
{
    Q_OBJECT
    
    Q_PROPERTY(QUrl source READ source WRITE setSource)
    
public:
    explicit Image(QWidget *parent = 0);
    ~Image();
    
    QUrl source() const;
    void setSource(const QUrl &url);
    
Q_SIGNALS:
    void clicked();
    
protected:
    void paintEvent(QPaintEvent *);
    
    void mousePressEvent(QMouseEvent *e);
    void mouseReleaseEvent(QMouseEvent *e);
    
    ImageCache *m_cache;
    
    QUrl m_source;
    
    bool m_pressed;
};  
    
#endif // IMAGE_H
