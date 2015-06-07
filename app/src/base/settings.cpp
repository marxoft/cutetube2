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

#include "settings.h"
#include "definitions.h"
#include "resources.h"
#include <QSettings>
#include <QNetworkProxy>
#ifdef CUTETUBE_DEBUG
#include <QDebug>
#endif

Settings* Settings::self = 0;

Settings::Settings(QObject *parent) :
    QObject(parent)
{
    if (!self) {
        self = this;
    }
}

Settings::~Settings() {
    if (self == this) {
        self = 0;
    }
}

Settings* Settings::instance() {
    return self;
}

#if (defined MEEGO_EDITION_HARMATTAN) || (SYMBIAN_OS)
QString Settings::activeColor() const {
    return value("Appearance/activeColor", "#0881cb").toString();
}

void Settings::setActiveColor(const QString &color) {
    if (color != activeColor()) {
        setValue("Appearance/activeColor", color);
        emit activeColorChanged();
    }
}

QString Settings::activeColorString() const {
    return value("Appearance/activeColorString", "color7").toString();
}

void Settings::setActiveColorString(const QString &s) {
    if (s != activeColorString()) {
        setValue("Appearance/activeColorString", s);
        emit activeColorStringChanged();
    }
}
#endif

QStringList Settings::categoryNames() const {
    QSettings settings;
    settings.beginGroup("Categories");
    QStringList names = settings.childKeys();
    names.prepend(tr("Default"));
    settings.endGroup();
    
    return names;
}

QList<Category> Settings::categories() const {
    QList<Category> list;
    QSettings settings;
    settings.beginGroup("Categories");
    
    foreach (QString key, settings.childKeys()) {
        Category category;
        category.name = key;
        category.path = settings.value(key).toString();
        list << category;
    }
    
    settings.endGroup();
    
    return list;
}

void Settings::setCategories(const QList<Category> &c) {
    QSettings settings;
    settings.remove("Categories");
    settings.beginGroup("Categories");
    
    foreach (Category category, c) {
        settings.setValue(category.name, category.path);
    }
    
    settings.endGroup();
    emit categoriesChanged();
}

void Settings::addCategory(const QString &name, const QString &path) {
    if (path != downloadPath(name)) {
        setValue("Categories/" + name, path);
        emit categoriesChanged();
    }
}

void Settings::removeCategory(const QString &name) {
    QSettings settings;
    settings.beginGroup("Categories");
    
    if (settings.contains(name)) {
        settings.remove(name);
        emit categoriesChanged();
    }
    
    settings.endGroup();
}

QString Settings::defaultCategory() const {
    return value("Transfers/defaultCategory", tr("Default")).toString();
}

void Settings::setDefaultCategory(const QString &category) {
    if (category != defaultCategory()) {
        setValue("Transfers/defaultCategory", category);
        emit defaultCategoryChanged();
    }
}

bool Settings::clipboardMonitorEnabled() const {
    return value("Content/clipboardMonitorEnabled", false).toBool();
}

void Settings::setClipboardMonitorEnabled(bool enabled) {
    if (enabled != clipboardMonitorEnabled()) {
        setValue("Content/clipboardMonitorEnabled", enabled);
        emit clipboardMonitorEnabledChanged();
    }
}

QString Settings::currentService() const {
    return value("Content/currentService", Resources::YOUTUBE).toString();
}

void Settings::setCurrentService(const QString &service) {
    if (service != currentService()) {
        setValue("Content/currentService", service);
        emit currentServiceChanged();
    }
}

QString Settings::defaultDownloadFormat(const QString &service) const {
    return value("DownloadFormats/" + service).toString();
}

void Settings::setDefaultDownloadFormat(const QString &service, const QString &format) {
    if (format != defaultDownloadFormat(service)) {
        setValue("DownloadFormats/" + service, format);
        emit downloadFormatsChanged();
    }
}

QString Settings::defaultPlaybackFormat(const QString &service) const {
    return value("PlaybackFormats/" + service).toString();
}

void Settings::setDefaultPlaybackFormat(const QString &service, const QString &format) {
    if (format != defaultPlaybackFormat(service)) {
        setValue("PlaybackFormats/" + service, format);
        emit playbackFormatsChanged();
    }
}

QString Settings::defaultSearchType(const QString &service) const {
    return value("Search/searchType/" + service).toString();
}

void Settings::setDefaultSearchType(const QString &service, const QString &type) {
    if (type != defaultSearchType(service)) {
        setValue("Search/searchType/" + service, type);
        emit defaultSearchTypeChanged();
    }
}

QString Settings::defaultViewMode() const {
    return value("Content/defaultViewMode", "list").toString();
}

void Settings::setDefaultViewMode(const QString &mode) {
    if (mode != defaultViewMode()) {
        setValue("Content/defaultViewMode", mode);
        emit defaultViewModeChanged();
    }
}

QString Settings::downloadPath() const {
    QString path = value("Transfers/downloadPath", DOWNLOAD_PATH).toString();

    if (!path.endsWith("/")) {
        path.append("/");
    }
    
    return path;
}

QString Settings::downloadPath(const QString &category) const {
    return value("Categories/" + category, downloadPath()).toString();
}   

void Settings::setDownloadPath(const QString &path) {
    if (path != downloadPath()) {
        setValue("Transfers/downloadPath", path);
        emit downloadPathChanged();
    }
}

QString Settings::locale() const {
    return value("Content/locale", QLocale().name()).toString();
}

void Settings::setLocale(const QString &name) {
    if (name != locale()) {
        setValue("Content/locale", name);
        emit localeChanged();
    }
}

int Settings::maximumConcurrentTransfers() const {
    return qBound(1, value("Transfers/maximumConcurrentTransfers", 1).toInt(), MAX_CONCURRENT_TRANSFERS);
}

void Settings::setMaximumConcurrentTransfers(int maximum) {
    if (maximum != maximumConcurrentTransfers()) {
        setValue("Transfers/maximumConcurrentTransfers", qBound(1, maximum, MAX_CONCURRENT_TRANSFERS));
        emit maximumConcurrentTransfersChanged();
    }
}

void Settings::setNetworkProxy() {
    if (!networkProxyEnabled()) {
        QNetworkProxy::setApplicationProxy(QNetworkProxy());
        return;
    }
    
    QNetworkProxy proxy(QNetworkProxy::ProxyType(networkProxyType()), networkProxyHost(), networkProxyPort(),
                        networkProxyUsername(), networkProxyPassword());
    
    QNetworkProxy::setApplicationProxy(proxy);
#ifdef CUTETUBE_DEBUG
    qDebug() << "Settings::setNetworkProxy" << networkProxyType() << networkProxyHost() << networkProxyPort()
                                            << networkProxyUsername() << networkProxyPassword();
#endif
}

bool Settings::networkProxyEnabled() const {
    return value("Network/networkProxyEnabled", false).toBool();
}

void Settings::setNetworkProxyEnabled(bool enabled) {
    if (enabled != networkProxyEnabled()) {
        setValue("Network/networkProxyEnabled", enabled);
        emit networkProxyChanged();
    }
}

QString Settings::networkProxyHost() const {
    return value("Network/networkProxyHost").toString();
}

void Settings::setNetworkProxyHost(const QString &host) {
    if (host != networkProxyHost()) {
        setValue("Network/networkProxyHost", host);
        emit networkProxyChanged();
    }
}

QString Settings::networkProxyPassword() const {
    return QByteArray::fromBase64(value("Network/networkProxyPassword").toByteArray());
}

void Settings::setNetworkProxyPassword(const QString &password) {
    QByteArray pass = password.toUtf8().toBase64();
    
    if (pass != networkProxyPassword()) {
        setValue("Network/networkProxyPassword", pass);
        emit networkProxyChanged();
    }
}

int Settings::networkProxyPort() const {
    return value("Network/networkProxyPort", 80).toInt();
}

void Settings::setNetworkProxyPort(int port) {
    if (port != networkProxyPort()) {
        setValue("Network/networkProxyPort", port);
        emit networkProxyChanged();
    }
}

int Settings::networkProxyType() const {
    return value("Network/networkProxyType", QNetworkProxy::ProxyType(QNetworkProxy::HttpProxy)).toInt();
}

void Settings::setNetworkProxyType(int type) {
    if (type != networkProxyType()) {
        setValue("Network/networkProxyType", type);
        emit networkProxyChanged();
    }
}

QString Settings::networkProxyUsername() const {
    return value("Network/networkProxyUsername").toString();
}

void Settings::setNetworkProxyUsername(const QString &username) {
    if (username != networkProxyUsername()) {
        setValue("Network/networkProxyUsername", username);
        emit networkProxyChanged();
    }
}

bool Settings::safeSearchEnabled() const {
    return value("Search/safeSearchEnabled", false).toBool();
}

void Settings::setSafeSearchEnabled(bool enabled) {
    if (enabled != safeSearchEnabled()) {
        setValue("Search/safeSearchEnabled", enabled);
        emit safeSearchEnabledChanged();
    }
}

int Settings::screenOrientation() const {
#ifdef Q_WS_MAEMO_5
    return value("Appearance/screenOrientation", Qt::WA_Maemo5LandscapeOrientation).toInt();
#else
    return value("Appearance/screenOrientation", 0).toInt();
#endif
}

void Settings::setScreenOrientation(int orientation) {
    if (orientation != screenOrientation()) {
        setValue("Appearance/screenOrientation", orientation);
        emit screenOrientationChanged();
    }
}

QStringList Settings::searchHistory() const {
    return value("Search/searchHistory").toStringList();
}

void Settings::setSearchHistory(const QStringList &searches) {
    setValue("Search/searchHistory", searches);
    emit searchHistoryChanged();
}

void Settings::addSearch(const QString &query) {
    QStringList searches = searchHistory();
    searches.removeOne(query);
    searches.prepend(query);
    setSearchHistory(searches);
}

void Settings::removeSearch(const QString &query) {
    QStringList searches = searchHistory();
    searches.removeOne(query);
    setSearchHistory(searches);
}

bool Settings::startTransfersAutomatically() const {
    return value("Transfers/startTransfersAutomatically", true).toBool();
}

void Settings::setStartTransfersAutomatically(bool enabled) {
    if (enabled != startTransfersAutomatically()) {
        setValue("Transfers/startTransfersAutomatically", enabled);
        emit startTransfersAutomaticallyChanged();
    }
}

bool Settings::subtitlesEnabled() const {
    return value("Content/subtitlesEnabled", false).toBool();
}

void Settings::setSubtitlesEnabled(bool enabled) {
    if (enabled != subtitlesEnabled()) {
        setValue("Content/subtitlesEnabled", enabled);
        emit subtitlesEnabledChanged();
    }
}

QString Settings::subtitlesLanguage() const {
    return value("Content/subtitlesLanguage").toString();
}

void Settings::setSubtitlesLanguage(const QString &language) {
    if (language != subtitlesLanguage()) {
        setValue("Content/subtitlesLanguage", language);
        emit subtitlesLanguageChanged();
    }
}

QVariant Settings::value(const QString &key, const QVariant &defaultValue) const {
    return QSettings().value(key, defaultValue);
}

void Settings::setValue(const QString &key, const QVariant &value) {
    QSettings().setValue(key, value);
#ifdef CUTETUBE_DEBUG
    qDebug() << "Settings::setValue" << key << value;
#endif
}

QString Settings::videoPlayer() const {
#ifdef SYMBIAN_OS
    return value("Content/videoPlayer", "mediaplayer").toString().toLower();
#else
    return value("Content/videoPlayer", "cutetube").toString().toLower();
#endif
}

void Settings::setVideoPlayer(const QString &player) {
    if (player != videoPlayer()) {
        setValue("Content/videoPlayer", player.toLower());
        emit videoPlayerChanged();
    }
}

#ifndef SYMBIAN_OS
QString Settings::videoPlayerCommand() const {
    return value("Content/videoPlayerCommand").toString();
}

void Settings::setVideoPlayerCommand(const QString &command) {
    if (command != videoPlayerCommand()) {
        setValue("Content/videoPlayerCommand", command);
    }
}
#endif
