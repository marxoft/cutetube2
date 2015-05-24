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
    Q_PROPERTY(Qt::AspectRatioMode aspectRatioMode READ aspectRatioMode WRITE setAspectRatioMode)
    Q_PROPERTY(Qt::TransformationMode transformationMode READ transformationMode WRITE setTransformationMode)
    
public:
    explicit Image(QWidget *parent = 0);
    ~Image();
    
    QUrl source() const;
    void setSource(const QUrl &url);
    
    Qt::AspectRatioMode aspectRatioMode() const;
    void setAspectRatioMode(Qt::AspectRatioMode mode);
    
    Qt::TransformationMode transformationMode() const;
    void setTransformationMode(Qt::TransformationMode mode);
    
Q_SIGNALS:
    void clicked();
    
protected:
    void paintEvent(QPaintEvent *);
    
    void mousePressEvent(QMouseEvent *e);
    void mouseReleaseEvent(QMouseEvent *e);
    
    ImageCache *m_cache;
    
    QUrl m_source;
    
    Qt::AspectRatioMode m_aspectRatioMode;
    Qt::TransformationMode m_transformationMode;
    
    bool m_pressed;
};  
    
#endif // IMAGE_H
