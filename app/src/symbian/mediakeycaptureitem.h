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
