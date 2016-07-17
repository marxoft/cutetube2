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

#include "filterbox.h"
#include <QAction>
#include <QLineEdit>

FilterBox::FilterBox(QWidget *parent) :
    QToolBar(parent),
    m_filterEdit(new QLineEdit(this)),
    m_cancelAction(addAction(QIcon::fromTheme("general_close"), QString(), this, SLOT(clear())))
{
    insertWidget(m_cancelAction, m_filterEdit);
    setFocusProxy(m_filterEdit);
    setAllowedAreas(Qt::BottomToolBarArea);

    connect(m_filterEdit, SIGNAL(textChanged(QString)), this, SIGNAL(textChanged(QString)));
}

QString FilterBox::text() const {
    return m_filterEdit->text();
}

void FilterBox::setText(const QString &text) {
    m_filterEdit->setText(text);
}

void FilterBox::clear() {
    m_filterEdit->clear();
}
