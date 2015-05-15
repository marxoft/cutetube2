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

#include "pluginvideodelegate.h"
#include "imagecache.h"
#include "pluginvideomodel.h"
#include <QPainter>
#include <QUrl>
#include <QApplication>
#include <QMouseEvent>

PluginVideoDelegate::PluginVideoDelegate(ImageCache *cache, QObject *parent) :
    QStyledItemDelegate(parent),
    m_cache(cache),
    m_gridMode(false),
    m_pressedRow(-1)
{
}

bool PluginVideoDelegate::editorEvent(QEvent *event, QAbstractItemModel *, const QStyleOptionViewItem &option,
                                      const QModelIndex &index) {

    if (event->type() == QEvent::MouseButtonPress) {
        QMouseEvent *mouse = static_cast<QMouseEvent*>(event);

        QRect imageRect = option.rect;
        imageRect.setLeft(imageRect.left() + (gridMode() ? 12 : 8));
        imageRect.setTop(imageRect.top() + 8);
        imageRect.setWidth(100);
        imageRect.setHeight(75);

        if (imageRect.contains(mouse->pos())) {
            m_pressedRow = index.row();
            return true;
        }
    }
    else if (event->type() == QEvent::MouseButtonRelease) {
        if (m_pressedRow == index.row()) {
            QMouseEvent *mouse = static_cast<QMouseEvent*>(event);

            QRect imageRect = option.rect;
            imageRect.setLeft(imageRect.left() + (gridMode() ? 12 : 8));
            imageRect.setTop(imageRect.top() + 8);
            imageRect.setWidth(100);
            imageRect.setHeight(75);

            if (imageRect.contains(mouse->pos())) {
                emit thumbnailClicked(index);
                m_pressedRow = -1;
                return true;
            }
        }
        
        m_pressedRow = -1;
    }

    return false;
}

void PluginVideoDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const {
    if (((option.state) & (QStyle::State_Selected)) && (m_pressedRow != index.row())) {
        painter->drawImage(option.rect, QImage("/etc/hildon/theme/images/TouchListBackgroundPressed.png"));
    }
    else if (!gridMode()) {
        painter->drawImage(option.rect, QImage("/etc/hildon/theme/images/TouchListBackgroundNormal.png"));
    }
    
    QUrl imageUrl = index.data(PluginVideoModel::ThumbnailUrlRole).toUrl();
    QImage image = m_cache->image(imageUrl, QSize(100, 75));
    
    QRect imageRect = option.rect;
    imageRect.setLeft(imageRect.left() + (gridMode() ? 12 : 8));
    imageRect.setTop(imageRect.top() + 8);
    imageRect.setWidth(100);
    imageRect.setHeight(75);
    
    if (!image.isNull()) {
        painter->drawImage(imageRect, image);
    }
    else {
        painter->fillRect(imageRect, Qt::black);
    }
    
    QString duration = index.data(PluginVideoModel::DurationRole).toString();
    QFont font;
    QRect durationRect = imageRect;
    const int durationHeight = durationRect.height() / 4;
    font.setPixelSize(durationHeight);        
    durationRect.setTop(durationRect.bottom() - (durationHeight));
    durationRect.setLeft(durationRect.right() - QFontMetrics(font).width(duration));

    painter->save();
    painter->setOpacity(0.8);
    painter->fillRect(durationRect, Qt::black);
    painter->setFont(font);
    painter->setOpacity(1);
    painter->setPen(Qt::white);
    painter->drawText(durationRect, Qt::AlignCenter, duration);
    painter->restore();
    
    painter->save();
    painter->setPen(QApplication::palette().color(QPalette::Mid));
    painter->drawRect(imageRect);
    painter->setPen(QApplication::palette().color(QPalette::Text));
    
    QRect textRect = option.rect;
    textRect.setLeft(textRect.left() + (gridMode() ? 8 : 116));
    textRect.setTop(gridMode() ? imageRect.bottom() + 8 : textRect.top() + 8);
    textRect.setRight(textRect.right() - 8);
    textRect.setBottom(textRect.bottom() - 8);
    
    QString text = index.data(PluginVideoModel::TitleRole).toString();
    
    if (gridMode()) {
        font.setPixelSize(16);
        painter->setFont(font);
        painter->drawText(textRect, Qt::AlignVCenter | Qt::TextWordWrap, text);
    }
    else {
        QString username = index.data(PluginVideoModel::UsernameRole).toString();
        QString date = index.data(PluginVideoModel::DateRole).toString();
        
        if (!username.isEmpty()) {
            text.append(QString("\n%1 %2").arg(tr("by")).arg(username));
        }
        
        if (!date.isEmpty()) {
            if (username.isEmpty()) {
                text.append("\n");
            }
            else {
                text.append(QString(" %1 ").arg(tr("on")));
            }
            
            text.append(date);
        }
        
        if (textRect.width() < 600) {
            font.setPixelSize(18);
            painter->setFont(font);
        }
        
        painter->drawText(textRect, Qt::AlignVCenter, text);
    }
    
    painter->restore();
}

QSize PluginVideoDelegate::sizeHint(const QStyleOptionViewItem &option, const QModelIndex &) const {
    return gridMode() ? QSize(124, 132) : QSize(option.rect.width(), 91);
}

bool PluginVideoDelegate::gridMode() const {
    return m_gridMode;
}

void PluginVideoDelegate::setGridMode(bool enabled) {
    m_gridMode = enabled;
}
