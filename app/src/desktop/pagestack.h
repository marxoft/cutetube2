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

#ifndef PAGESTACK_H
#define PAGESTACK_H

#include <QWidget>

class Page;

class PageStack : public QWidget
{
    Q_OBJECT

    Q_PROPERTY(int count READ count NOTIFY countChanged)

public:
    explicit PageStack(QWidget *parent = 0) : QWidget(parent) {}

    virtual int count() const = 0;
    
    virtual Page* currentPage() const = 0;

public Q_SLOTS:
    virtual void push(Page *page) = 0;
    virtual void replace(Page *page) = 0;
    virtual void pop() = 0;
    virtual void clear() = 0;

Q_SIGNALS:
    void countChanged(int count);
    void currentChanged(Page *page);
};

#endif // PAGESTACK_H
