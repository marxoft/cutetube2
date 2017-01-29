/*
 * Copyright (C) 2015 Stuart Howarth <showarth@marxoft.co.uk>
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

#include "commentdelegate.h"
#include "drawing.h"
#include "imagecache.h"
#include <QPainter>
#include <QApplication>
#include <QMouseEvent>

CommentDelegate::CommentDelegate(ImageCache *cache, int bodyRole, int dateRole, int thumbnailRole, int usernameRole,
                                 QObject *parent) :
    QStyledItemDelegate(parent),
    m_cache(cache),
    m_bodyRole(bodyRole),
    m_dateRole(dateRole),
    m_thumbnailRole(thumbnailRole),
    m_usernameRole(usernameRole),
    m_pressedRow(-1)
{
}

bool CommentDelegate::editorEvent(QEvent *event, QAbstractItemModel *, const QStyleOptionViewItem &option,
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

void CommentDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option,
                                  const QModelIndex &index) const {
        
    QRect imageRect = option.rect;
    imageRect.setLeft(imageRect.left() + 8);
    imageRect.setTop(imageRect.top() + 8);
    imageRect.setWidth(40);
    imageRect.setHeight(40);
    
    QImage image = m_cache->image(index.data(m_thumbnailRole).toString(), QSize(40, 40));
    
    if (image.isNull()) {
        image = QImage("/usr/share/icons/hicolor/48x48/hildon/general_default_avatar.png")
            .scaled(40, 40, Qt::KeepAspectRatio, Qt::SmoothTransformation);
    }
    
    drawCenteredImage(painter, imageRect, image);
    
    QRect textRect = option.rect;
    textRect.setLeft(imageRect.right() + 8);
    textRect.setRight(textRect.right() - 8);
    textRect.setTop(textRect.top() + 8);
    textRect.setHeight(40);
    
    painter->save();
    QFont font;
    font.setPointSize(13);
    painter->setFont(font);
    painter->setPen(QApplication::palette().color(QPalette::Mid));
    painter->drawText(textRect, Qt::AlignVCenter | Qt::TextWordWrap,
                      QString(tr("by %1 on %2").arg(index.data(m_usernameRole).toString())
                                               .arg(index.data(m_dateRole).toString())));
    
    textRect = option.rect;
    textRect.setLeft(imageRect.left());
    textRect.setRight(textRect.right() - 8);
    textRect.setTop(imageRect.bottom() + 8);
    textRect.setBottom(textRect.bottom() - 8);
    
    painter->restore();
    painter->drawText(textRect, Qt::TextWordWrap, index.data(m_bodyRole).toString());
}

QSize CommentDelegate::sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const {
    return QSize(option.rect.width(), option.fontMetrics.boundingRect(QRect(option.rect.left() + 8,
                                                                            option.rect.top() + 56,
                                                                            option.rect.width() - 16,
                                                                            1000),
                 Qt::TextWordWrap, index.data(m_bodyRole).toString()).height() + 64);
}
