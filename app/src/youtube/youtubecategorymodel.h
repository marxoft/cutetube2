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

#ifndef YOUTUBECATEGORYMODEL_H
#define YOUTUBECATEGORYMODEL_H

#include "selectionmodel.h"
#include <qyoutube/resourcesrequest.h>
#include <QStringList>

class YouTubeCategoryModel : public SelectionModel
{
    Q_OBJECT
    
    Q_PROPERTY(QString errorString READ errorString NOTIFY statusChanged)
    Q_PROPERTY(QYouTube::ResourcesRequest::Status status READ status NOTIFY statusChanged)
    
public:
    explicit YouTubeCategoryModel(QObject *parent = 0);
    
    QString errorString() const;
    
    QYouTube::ResourcesRequest::Status status() const;
    
    Q_INVOKABLE void list(const QString &resourcePath, const QStringList &part, const QVariantMap &filters,
                          const QVariantMap &params = QVariantMap());
            
public Q_SLOTS:
    void cancel();
    void reload();

private Q_SLOTS:
    void onRequestFinished();
    
Q_SIGNALS:
    void statusChanged(QYouTube::ResourcesRequest::Status s);
        
private:
    QYouTube::ResourcesRequest *m_request;
    
    QString m_resourcePath;
    QStringList m_part;
    QVariantMap m_filters;
    QVariantMap m_params;
};
    
#endif // YOUTUBECATEGORYMODEL_H
