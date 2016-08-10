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

#ifndef PAGE_H
#define PAGE_H

#include <QWidget>
#include <QPointer>

class PageStack;

class Page : public QWidget
{
    Q_OBJECT

    Q_PROPERTY(Status status READ status NOTIFY statusChanged)
    Q_PROPERTY(QString statusText READ statusText NOTIFY statusTextChanged)

    Q_ENUMS(Status)

public:
    enum Status {
        Null = 0,
        Loading,
        Ready
    };
    
    explicit Page(QWidget *parent = 0);

    PageStack *stack() const;
    void setStack(PageStack *stack);

    virtual Status status() const;
    virtual QString statusText() const;

public Q_SLOTS:
    virtual void cancel();
    virtual void reload();
    virtual void toggleLoading();

protected:
    virtual bool event(QEvent *e);
    
Q_SIGNALS:
    void statusChanged(Page::Status status);
    void statusTextChanged(const QString &text);
    void windowTitleChanged(const QString &text);

private:    
    QPointer<PageStack> m_stack;
};

#endif // PAGE_H
