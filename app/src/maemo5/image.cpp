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

#include "image.h"
#include "imagecache.h"
#include <QPainter>
#include <QMouseEvent>

Image::Image(QWidget *parent) :
    QWidget(parent),
    m_cache(new ImageCache)
{
    connect(m_cache, SIGNAL(imageReady()), this, SLOT(update()));
}

Image::~Image() {
    delete m_cache;
    m_cache = 0;
}

QUrl Image::source() const {
    return m_source;
}

void Image::setSource(const QUrl &url) {
    m_source = url;
    update();
}

void Image::paintEvent(QPaintEvent *) {
    QPainter painter(this);
    
    if (source().isValid()) {    
        QImage image = m_cache->image(source(), size());
        
        if (!image.isNull()) {
            painter.drawImage(rect(), image);
            return;
        }
    }
    
    painter.fillRect(rect(), Qt::black);
}

void Image::mousePressEvent(QMouseEvent *e) {
    m_pressed = true;
    e->accept();
}

void Image::mouseReleaseEvent(QMouseEvent *e) {
    if ((m_pressed) && (rect().contains(e->pos()))) {
        emit clicked();
        e->accept();
    }

    m_pressed = false;
}
