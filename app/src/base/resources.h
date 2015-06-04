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

#ifndef RESOURCES_H
#define RESOURCES_H

#include <QObject>
#include <QVariantMap>

class Resources : public QObject
{
    Q_OBJECT
    
    Q_PROPERTY(QString YOUTUBE READ youtubeConstant CONSTANT)
    Q_PROPERTY(QString DAILYMOTION READ dailymotionConstant CONSTANT)
    Q_PROPERTY(QString VIMEO READ vimeoConstant CONSTANT)
    
    Q_PROPERTY(QString DATE READ dateConstant CONSTANT)
    Q_PROPERTY(QString RELEVANCE READ relevanceConstant CONSTANT)
    Q_PROPERTY(QString POPULAR READ popularConstant CONSTANT)
    Q_PROPERTY(QString ALPHABET READ alphabetConstant CONSTANT)
    
    Q_PROPERTY(QString VIDEO READ videoConstant CONSTANT)
    Q_PROPERTY(QString PLAYLIST READ playlistConstant CONSTANT)
    Q_PROPERTY(QString USER READ userConstant CONSTANT)
    Q_PROPERTY(QString GROUP READ groupConstant CONSTANT)
    Q_PROPERTY(QString CATEGORY READ categoryConstant CONSTANT)
    Q_PROPERTY(QString COMMENT READ commentConstant CONSTANT)
    Q_PROPERTY(QString STREAM READ streamConstant CONSTANT)
    Q_PROPERTY(QString SUBTITLE READ subtitleConstant CONSTANT)
        
public:
    explicit Resources(QObject *parent = 0);
    
    static const QString YOUTUBE;
    static const QString DAILYMOTION;
    static const QString VIMEO;

    static const QString DATE;
    static const QString RELEVANCE;
    static const QString POPULAR;
    static const QString ALPHABET;

    static const QString VIDEO;
    static const QString PLAYLIST;
    static const QString USER;
    static const QString GROUP;
    static const QString CATEGORY;
    static const QString COMMENT;
    static const QString STREAM;
    static const QString SUBTITLE;
        
    static QString youtubeConstant();
    static QString dailymotionConstant();
    static QString vimeoConstant();
    
    static QString dateConstant();
    static QString relevanceConstant();
    static QString popularConstant();
    static QString alphabetConstant();
    
    static QString videoConstant();
    static QString playlistConstant();
    static QString userConstant();
    static QString groupConstant();
    static QString categoryConstant();
    static QString commentConstant();
    static QString streamConstant();
    static QString subtitleConstant();
    
    Q_INVOKABLE static QVariantMap getResourceFromUrl(QString url);    
};

#endif // RESOURCES_H
