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

#ifndef VIMEOCATEGORYMODEL_H
#define VIMEOCATEGORYMODEL_H

#include "selectionmodel.h"
#include <qvimeo/resourcesrequest.h>

class VimeoCategoryModel : public SelectionModel
{
    Q_OBJECT
    
    Q_PROPERTY(QString errorString READ errorString NOTIFY statusChanged)
    Q_PROPERTY(QVimeo::ResourcesRequest::Status status READ status NOTIFY statusChanged)
    
public:
    explicit VimeoCategoryModel(QObject *parent = 0);
    
    QString errorString() const;
    
    QVimeo::ResourcesRequest::Status status() const;
    
    Q_INVOKABLE void list(const QString &resourcePath, const QVariantMap &filters = QVariantMap());
            
public Q_SLOTS:
    void cancel();
    void reload();

private Q_SLOTS:
    void onRequestFinished();
    
Q_SIGNALS:
    void statusChanged(QVimeo::ResourcesRequest::Status s);
        
private:
    QVimeo::ResourcesRequest *m_request;
    
    QString m_resourcePath;
    QVariantMap m_filters;
};
    
#endif // VIMEOCATEGORYMODEL_H
