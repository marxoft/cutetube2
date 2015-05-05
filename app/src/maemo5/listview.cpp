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

#include "listview.h"
#include <QMouseEvent>
#include <QScrollBar>
#include <QAbstractKineticScroller>

ListView::ListView(QWidget *parent) :
    QListView(parent),
    m_kineticScroller(property("kineticScroller").value<QAbstractKineticScroller *>()),
    m_clearCurrent(true)
{
    setUniformItemSizes(true);
    setAutoScroll(false);
    setEditTriggers(QListView::NoEditTriggers);
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
}

bool ListView::clearCurrentIndexOnMouseRelease() const {
    return m_clearCurrent;
}

void ListView::setClearCurrentIndexOnMouseRelease(bool enabled) {
    m_clearCurrent = enabled;
    
    if (enabled) {
        setCurrentIndex(QModelIndex());
    }
}

void ListView::positionAtBeginning() {
    scrollTo(model()->index(0, 0), QListView::PositionAtTop);
}

void ListView::positionAtEnd() {
    scrollTo(model()->index(model()->rowCount() - 1, 0), QListView::PositionAtBottom);
}

void ListView::keyPressEvent(QKeyEvent *e) {
    if (e->key() == Qt::Key_Down) {
        if (e->modifiers() == Qt::ShiftModifier) {
            positionAtEnd();
        }
        else {
            m_kineticScroller->scrollTo(QPoint(0, rectForIndex(indexAt(QPoint(0, height()))).y()));
        }

        e->accept();
    }
    else if (e->key() == Qt::Key_Up) {
        if (e->modifiers() == Qt::ShiftModifier) {
            positionAtBeginning();
        }
        else {
            m_kineticScroller->scrollTo(QPoint(0, rectForIndex(indexAt(QPoint(0, -height()))).y()));
        }

        e->accept();
    }
    else {
        e->ignore();
    }
}

void ListView::mousePressEvent(QMouseEvent *e) {
    setCurrentIndex(indexAt(e->pos()));
    QListView::mousePressEvent(e);
}

void ListView::mouseReleaseEvent(QMouseEvent *e) {
    QListView::mouseReleaseEvent(e);
    setCurrentIndex(QModelIndex());
}
