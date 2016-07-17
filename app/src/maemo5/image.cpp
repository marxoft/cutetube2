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

#include "image.h"
#include "drawing.h"
#include "imagecache.h"
#include <QPainter>
#include <QMouseEvent>

Image::Image(QWidget *parent) :
    QWidget(parent),
    m_cache(new ImageCache),
    m_aspectRatioMode(Qt::KeepAspectRatio),
    m_transformationMode(Qt::SmoothTransformation)
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
    if (url != source()) {
        m_source = url;
        update();
    }
}

Qt::AspectRatioMode Image::aspectRatioMode() const {
    return m_aspectRatioMode;
}

void Image::setAspectRatioMode(Qt::AspectRatioMode mode) {
    if (mode != aspectRatioMode()) {
        m_aspectRatioMode = mode;
        update();
    }
}

Qt::TransformationMode Image::transformationMode() const {
    return m_transformationMode;
}

void Image::setTransformationMode(Qt::TransformationMode mode) {
    if (mode != transformationMode()) {
        m_transformationMode = mode;
        update();
    }
}

void Image::paintEvent(QPaintEvent *) {
    QPainter painter(this);
    
    if (source().isValid()) {    
        const QImage image = m_cache->image(source(), size(), aspectRatioMode(), transformationMode());
        
        if (!image.isNull()) {
            drawCenteredImage(&painter, rect(), image);
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
