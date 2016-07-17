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

#ifndef STACKEDWINDOW_H
#define STACKEDWINDOW_H

#include <QMainWindow>

class StackedWindow : public QMainWindow
{
    Q_OBJECT
    
    Q_PROPERTY(bool busy READ isBusy)
    
public:
    explicit StackedWindow(StackedWindow *parent = 0);
    ~StackedWindow();
    
    static StackedWindow* currentWindow();
    static void popWindow();
    static void clearWindows();
    
    bool isBusy() const;
    
public Q_SLOTS:
    void showProgressIndicator();
    void hideProgressIndicator();
    
protected:
    virtual void showEvent(QShowEvent *e);
    
    static QList<StackedWindow*> stack;
    
private:
    int m_progressCount;
};

#endif // STACKEDWINDOW_H
