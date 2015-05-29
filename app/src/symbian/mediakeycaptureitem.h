/*
 * Copyright (C) 2014 Stuart Howarth <showarth@marxoft.co.uk>
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms and conditions of the GNU Lesser General Public License,
 * version 3, as published by the Free Software Foundation.
 *
 * This program is distributed in the hope it will be useful, but WITHOUT ANY
 * WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for
 * more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program; if not, write to the Free Software Foundation,
 * Inc., 51 Franklin St - Fifth Floor, Boston, MA 02110-1301 USA.
 */

#ifndef MediakeyCaptureItem_H
#define MediakeyCaptureItem_H
 
#include <QObject>
#include <qdeclarative.h>
#include <remconcoreapitargetobserver.h>    // link against RemConCoreApi.lib
#include <remconcoreapitarget.h>            // and
#include <remconinterfaceselector.h>        // RemConInterfaceBase.lib
 
class QTimer;
class MediakeyCaptureItemPrivate;
class MediakeyCaptureItem : public QObject
{
    Q_OBJECT

    Q_PROPERTY(qreal volume READ volume NOTIFY volumeChanged)

public:
    MediakeyCaptureItem(QObject *parent = 0);
    qreal volume() const;
    void setVolume(qreal volume);

private:
    void increaseVolume();
    void decreaseVolume();

private slots:
    void onTimerTriggered();

signals:
    void volumeChanged();
 
private:
    MediakeyCaptureItemPrivate *d_ptr;

private:    // Friend class definitions
    friend class MediakeyCaptureItemPrivate;
};

QML_DECLARE_TYPE(MediakeyCaptureItem)
 
#endif // MediakeyCaptureItem_H
