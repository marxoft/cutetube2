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

#include "page.h"
#include "pagestack.h"
#include <QEvent>

Page::Page(QWidget *parent) :
    QWidget(parent)
{
}

PageStack* Page::stack() const {
    return m_stack;
}

void Page::setStack(PageStack *stack) {
    m_stack = stack;
}

Page::Status Page::status() const {
    return Null;
}

QString Page::statusText() const {
    return QString();
}

void Page::cancel() {}

void Page::reload() {}

void Page::toggleLoading() {
    if (status() == Loading) {
        cancel();
    }
    else {
        reload();
    }
}

bool Page::event(QEvent *e) {
    if (e->type() == QEvent::WindowTitleChange) {
        emit windowTitleChanged(windowTitle());
    }
    
    return QWidget::event(e);
}
