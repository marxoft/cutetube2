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

#include "resources.h"
#include "dailymotion.h"
#include "resourcesplugins.h"
#include "utils.h"
#include "vimeo.h"
#include "youtube.h"

const QString Resources::YOUTUBE("youtube");
const QString Resources::DAILYMOTION("dailymotion");
const QString Resources::VIMEO("vimeo");

const QString Resources::DATE("date");
const QString Resources::RELEVANCE("relevance");
const QString Resources::POPULAR("popular");
const QString Resources::ALPHABET("alphabet");

const QString Resources::VIDEO("video");
const QString Resources::PLAYLIST("playlist");
const QString Resources::USER("user");
const QString Resources::GROUP("group");
const QString Resources::CATEGORY("category");
const QString Resources::COMMENT("comment");
const QString Resources::STREAM("stream");
const QString Resources::SUBTITLE("subtitle");

Resources::Resources(QObject *parent) :
    QObject(parent)
{
}

QString Resources::youtubeConstant() {
    return YOUTUBE;
}

QString Resources::dailymotionConstant() {
    return DAILYMOTION;
}

QString Resources::vimeoConstant() {
    return VIMEO;
}

QString Resources::dateConstant() {
    return DATE;
}

QString Resources::relevanceConstant() {
    return RELEVANCE;
}

QString Resources::popularConstant() {
    return POPULAR;
}

QString Resources::alphabetConstant() {
    return ALPHABET;
}

QString Resources::videoConstant() {
    return VIDEO;
}

QString Resources::playlistConstant() {
    return PLAYLIST;
}

QString Resources::userConstant() {
    return USER;
}

QString Resources::groupConstant() {
    return GROUP;
}

QString Resources::categoryConstant() {
    return CATEGORY;
}

QString Resources::commentConstant() {
    return COMMENT;
}

QString Resources::streamConstant() {
    return STREAM;
}

QString Resources::subtitleConstant() {
    return SUBTITLE;
}

QVariantMap Resources::getResourceFromUrl(QString url) {
    url = Utils::unescape(url);
    QVariantMap result;

    if (YouTube::URL_REGEXP.indexIn(url) == 0) {
        result.insert("service", YOUTUBE);
        result.insert("id", url.section(QRegExp("v=|list=|/"), -1).section(QRegExp("&|\\?"), 0, 0));

        if ((url.contains("youtu.be")) || (url.contains("v=") || (url.contains("/v/")))) {
            result.insert("type", VIDEO);
        }
        else if (url.contains("list=")) {
            result.insert("type", PLAYLIST);
        }
        else {
            result.insert("type", USER);
        }
    }
    else if (Dailymotion::URL_REGEXP.indexIn(url) == 0) {
        result.insert("service", DAILYMOTION);
        result.insert("id", url.section('/', -1).section('_', 0, 0));

        if ((url.contains("dai.ly") || (url.contains("/video/")))) {
            result.insert("type", VIDEO);
        }
        else if (url.contains("/playlist/")) {
            result.insert("type", PLAYLIST);
        }
        else {
            result.insert("type", USER);
        }
    }
    else if (Vimeo::URL_REGEXP.indexIn(url) == 0) {
        QString id = url.section('/', -1);

        result.insert("service", VIMEO);
        result.insert("id", id);

        if (url.contains("/album/")) {
            result.insert("type", PLAYLIST);
        }
        else if (id.contains(QRegExp("\\D+"))) {
            result.insert("type", USER);
        }
        else {
            result.insert("type", VIDEO);
        }
    }
    else {
        ResourcesPlugin plugin = ResourcesPlugins::instance()->getPluginFromUrl(url);
        
        if (!plugin.name.isEmpty()) {
            result.insert("service", plugin.name);
            result.insert("id", url);
            result.insert("type", VIDEO);
        }
    }
    
    return result;
}
