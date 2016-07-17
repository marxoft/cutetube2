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

#ifndef LISTVIEW_H
#define LISTVIEW_H

#include <QListView>

class QAbstractKineticScroller;

class ListView : public QListView
{
    Q_OBJECT
    
    Q_PROPERTY(bool clearCurrentIndexOnMouseRelease READ clearCurrentIndexOnMouseRelease
               WRITE setClearCurrentIndexOnMouseRelease)
    
public:
    explicit ListView(QWidget *parent = 0);
    
    bool clearCurrentIndexOnMouseRelease() const;
    void setClearCurrentIndexOnMouseRelease(bool enabled);
    
public Q_SLOTS:
    void positionAtBeginning();
    void positionAtEnd();
    
protected:
    void keyPressEvent(QKeyEvent *e);
    void mousePressEvent(QMouseEvent *e);
    void mouseReleaseEvent(QMouseEvent *e);

private:
    QAbstractKineticScroller *m_kineticScroller;
    bool m_clearCurrent;
};

#endif // LISTVIEW_H
