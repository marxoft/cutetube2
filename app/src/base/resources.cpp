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

#include "resources.h"
#include "dailymotion.h"
#include "logger.h"
#include "pluginmanager.h"
#include "utils.h"
#include "vimeo.h"
#include "youtube.h"

const QString Resources::YOUTUBE("youtube");
const QString Resources::DAILYMOTION("dailymotion");
const QString Resources::VIMEO("vimeo");

const QString Resources::VIDEO("video");
const QString Resources::PLAYLIST("playlist");
const QString Resources::USER("user");
const QString Resources::GROUP("group");
const QString Resources::CATEGORY("category");
const QString Resources::COMMENT("comment");
const QString Resources::STREAM("stream");
const QString Resources::SUBTITLE("subtitle");

GetResource::GetResource(const QVariantMap &map) :
    QVariantMap()
{
    insert("type", map.value("type"));
    insert("regExp", map.value("regExp"));
}

GetResource::GetResource(const QString &type, const QRegExp &regExp) :
    QVariantMap()
{
    setType(type);
    setRegExp(regExp);
}

QRegExp GetResource::regExp() const {
    return QRegExp(value("regExp").toString());
}

void GetResource::setRegExp(const QRegExp &regExp) {
    insert("regExp", regExp);
}

QString GetResource::type() const {
    return value("type").toString();
}

void GetResource::setType(const QString &type) {
    insert("type", type);
}

ListResource::ListResource(const QVariantMap &map) :
    QVariantMap()
{
    insert("label", map.value("label"));
    insert("type", map.value("type"));
    insert("id", map.value("id"));
}

ListResource::ListResource(const QString &label, const QString &type, const QString &id) :
    QVariantMap()
{
    setId(id);
    setLabel(label);
    setType(type);
}

QString ListResource::id() const {
    return value("id").toString();
}

void ListResource::setId(const QString &id) {
    insert("id", id);
}

QString ListResource::label() const {
    return value("label").toString();
}

void ListResource::setLabel(const QString &label) {
    insert("label", label);
}

QString ListResource::type() const {
    return value("type").toString();
}

void ListResource::setType(const QString &type) {
    insert("type", type);
}

SearchResource::SearchResource(const QVariantMap &map) :
    QVariantMap()
{
    insert("label", map.value("label"));
    insert("type", map.value("type"));
    insert("order", map.value("order"));
}

SearchResource::SearchResource(const QString &label, const QString &type, const QString &order) :
    QVariantMap()
{
    setLabel(label);
    setType(type);
    setOrder(order);
}

QString SearchResource::label() const {
    return value("label").toString();
}

void SearchResource::setLabel(const QString &label) {
    insert("label", label);
}

QString SearchResource::order() const {
    return value("order").toString();
}

void SearchResource::setOrder(const QString &order) {
    insert("order", order);
}

QString SearchResource::type() const {
    return value("type").toString();
}

void SearchResource::setType(const QString &type) {
    insert("type", type);
}

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
    Logger::log("Resources::getResourceFromUrl. URL: " + url, Logger::MediumVerbosity);
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
        const QString id = url.section('/', -1);

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
        const QList<ServicePluginPair> plugins = PluginManager::instance()->plugins();

        foreach (const ServicePluginPair &pair, plugins) {
            if (const ServicePluginConfig *config = pair.config) {
                
                foreach (const GetResource &resource, config->getResources()) {                    
                    if (resource.regExp().indexIn(url) == 0) {
                        result.insert("service", config->id());
                        result.insert("type", resource.type());
                        result.insert("id", url);
                        return result;
                    }
                }
            }
        }
    }
    
    return result;
}
