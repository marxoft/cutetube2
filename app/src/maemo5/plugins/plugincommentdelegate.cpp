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

#include "plugincommentdelegate.h"
#include "imagecache.h"
#include "plugincommentmodel.h"
#include <QPainter>
#include <QUrl>
#include <QApplication>
#include <QMouseEvent>

PluginCommentDelegate::PluginCommentDelegate(ImageCache *cache, QObject *parent) :
    QStyledItemDelegate(parent),
    m_cache(cache),
    m_pressedRow(-1)
{
}

bool PluginCommentDelegate::editorEvent(QEvent *event, QAbstractItemModel *, const QStyleOptionViewItem &option,
                                        const QModelIndex &index) {

    if (event->type() == QEvent::MouseButtonPress) {
        QMouseEvent *mouse = static_cast<QMouseEvent*>(event);

        QRect imageRect = option.rect;
        imageRect.setLeft(imageRect.left() + 8);
        imageRect.setTop(imageRect.top() + 8);
        imageRect.setWidth(40);
        imageRect.setHeight(40);

        if (imageRect.contains(mouse->pos())) {
            m_pressedRow = index.row();
            return true;
        }
    }
    else if (event->type() == QEvent::MouseButtonRelease) {
        if (m_pressedRow == index.row()) {
            QMouseEvent *mouse = static_cast<QMouseEvent*>(event);

            QRect imageRect = option.rect;
            imageRect.setLeft(imageRect.left() + 8);
            imageRect.setTop(imageRect.top() + 8);
            imageRect.setWidth(40);
            imageRect.setHeight(40);

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

void PluginCommentDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option,
                                  const QModelIndex &index) const {
    
    qDrawBorderPixmap(painter, option.rect, QMargins(30, 60, 30, 30),
                      QPixmap("/etc/hildon/theme/images/ContactsAppletBubble.png"));
    
    QUrl imageUrl = index.data(PluginCommentModel::ThumbnailUrlRole).toUrl();
    QImage image = m_cache->image(imageUrl, QSize(40, 40));
    
    QRect imageRect = option.rect;
    imageRect.setLeft(imageRect.left() + 24);
    imageRect.setTop(imageRect.top() + 16);
    imageRect.setWidth(40);
    imageRect.setHeight(40);
    
    if (!image.isNull()) {
        painter->drawImage(imageRect, image);
    }
    else {
        painter->fillRect(imageRect, Qt::black);
    }
    
    QRect textRect = option.rect;
    textRect.setLeft(imageRect.right() + 8);
    textRect.setRight(textRect.right() - 16);
    textRect.setTop(textRect.top() + 8);
    textRect.setHeight(40);
    
    QFont font;
    font.setPixelSize(18);
    
    painter->save();
    painter->setFont(font);
    painter->setPen(QApplication::palette().color(QPalette::Mid));
    painter->drawText(textRect, Qt::AlignVCenter | Qt::TextWordWrap,
                      QString(tr("by %1 on %2").arg(index.data(PluginCommentModel::UsernameRole).toString())
                                               .arg(index.data(PluginCommentModel::DateRole).toString())));
    
    textRect = option.rect;
    textRect.setLeft(imageRect.left());
    textRect.setRight(textRect.right() - 16);
    textRect.setTop(imageRect.bottom() + 8);
    textRect.setBottom(textRect.bottom() - 16);
    
    painter->setPen(QApplication::palette().color(QPalette::Dark));
    painter->drawText(textRect, Qt::TextWordWrap, index.data(PluginCommentModel::BodyRole).toString());
    painter->restore();
}

QSize PluginCommentDelegate::sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const {
    return QSize(option.rect.width(), option.fontMetrics.boundingRect(option.rect, Qt::TextWordWrap,
                 index.data(PluginCommentModel::BodyRole).toString()).height() + 80);
}
