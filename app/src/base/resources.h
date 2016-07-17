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

#ifndef RESOURCES_H
#define RESOURCES_H

#include <QObject>
#include <QRegExp>
#include <QVariantMap>

class GetResource : public QVariantMap
{

public:
    explicit GetResource(const QVariantMap &map);
    explicit GetResource(const QString &type, const QRegExp &regExp);

    QRegExp regExp() const;
    void setRegExp(const QRegExp &regExp);

    QString type() const;
    void setType(const QString &type);
};

class ListResource : public QVariantMap
{

public:
    explicit ListResource(const QVariantMap &map);
    explicit ListResource(const QString &label, const QString &type, const QString &id);

    QString id() const;
    void setId(const QString &id);

    QString label() const;
    void setLabel(const QString &label);

    QString type() const;
    void setType(const QString &type);
};

class SearchResource : public QVariantMap
{

public:
    explicit SearchResource(const QVariantMap &map);
    explicit SearchResource(const QString &label, const QString &type, const QString &order);

    QString label() const;
    void setLabel(const QString &label);

    QString order() const;
    void setOrder(const QString &order);

    QString type() const;
    void setType(const QString &type);
};

class Resources : public QObject
{
    Q_OBJECT
    
    Q_PROPERTY(QString YOUTUBE READ youtubeConstant CONSTANT)
    Q_PROPERTY(QString DAILYMOTION READ dailymotionConstant CONSTANT)
    Q_PROPERTY(QString VIMEO READ vimeoConstant CONSTANT)
    
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
