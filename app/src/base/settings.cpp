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
        QSettings().setValue("Categories/" + name, path);
        emit categoriesChanged();
    }
#ifdef CUTETUBE_DEBUG
    qDebug() << "Settings::addCategory" << name << path;
#endif
}

void Settings::removeCategory(const QString &name) {
    QSettings settings;
    settings.beginGroup("Categories");
    
    if (settings.contains(name)) {
        settings.remove(name);
        emit categoriesChanged();
    }
    
    settings.endGroup();
#ifdef CUTETUBE_DEBUG
    qDebug() << "Settings::removeCategory" << name;
#endif
}

QString Settings::defaultCategory() const {
    return QSettings().value("Transfers/defaultCategory", tr("Default")).toString();
}

void Settings::setDefaultCategory(const QString &category) {
    if (category != defaultCategory()) {
        QSettings().setValue("Transfers/defaultCategory", category);
        emit defaultCategoryChanged();
    }
#ifdef CUTETUBE_DEBUG
    qDebug() << "Settings::setDefaultCategory" << category;
#endif
}

bool Settings::clipboardMonitorEnabled() const {
    return QSettings().value("Content/clipboardMonitorEnabled", false).toBool();
}

void Settings::setClipboardMonitorEnabled(bool enabled) {
    if (enabled != clipboardMonitorEnabled()) {
        QSettings().setValue("Content/clipboardMonitorEnabled", enabled);
        emit clipboardMonitorEnabledChanged();
    }
#ifdef CUTETUBE_DEBUG
    qDebug() << "Settings::setClipboardMonitorEnabled" << enabled;
#endif
}

QString Settings::currentService() const {
    return QSettings().value("Content/currentService", Resources::YOUTUBE).toString();
}

void Settings::setCurrentService(const QString &service) {
    if (service != currentService()) {
        QSettings().setValue("Content/currentService", service);
        emit currentServiceChanged();
    }
#ifdef CUTETUBE_DEBUG
    qDebug() << "Settings::setCurrentService" << service;
#endif
}

QString Settings::defaultDownloadFormat(const QString &service) const {
    return QSettings().value("DownloadFormats/" + service).toString();
}

void Settings::setDefaultDownloadFormat(const QString &service, const QString &format) {
    if (format != defaultDownloadFormat(service)) {
        QSettings().setValue("DownloadFormats/" + service, format);
        emit downloadFormatsChanged();
    }
#ifdef CUTETUBE_DEBUG
    qDebug() << "Settings::setDefaultDownloadFormat" << service << format;
#endif
}

QString Settings::defaultPlaybackFormat(const QString &service) const {
    return QSettings().value("PlaybackFormats/" + service).toString();
}

void Settings::setDefaultPlaybackFormat(const QString &service, const QString &format) {
    if (format != defaultPlaybackFormat(service)) {
        QSettings().setValue("PlaybackFormats/" + service, format);
        emit playbackFormatsChanged();
    }
#ifdef CUTETUBE_DEBUG
    qDebug() << "Settings::setDefaultPlaybackFormat" << service << format;
#endif
}

QString Settings::defaultViewMode() const {
    return QSettings().value("Content/defaultViewMode", "list").toString();
}

void Settings::setDefaultViewMode(const QString &mode) {
    if (mode != defaultViewMode()) {
        QSettings().setValue("Content/defaultViewMode", mode);
        emit defaultViewModeChanged();
    }
#ifdef CUTETUBE_DEBUG
    qDebug() << "Settings::setDefaultViewMode" << mode;
#endif
}

QString Settings::downloadPath() const {
    QString path = QSettings().value("Transfers/downloadPath", DOWNLOAD_PATH).toString();

    if (!path.endsWith("/")) {
        path.append("/");
    }
    
    return path;
}

QString Settings::downloadPath(const QString &category) const {
    return QSettings().value("Categories/" + category, downloadPath()).toString();
}   

void Settings::setDownloadPath(const QString &path) {
    if (path != downloadPath()) {
        QSettings().setValue("Transfers/downloadPath", path);
        emit downloadPathChanged();
    }
#ifdef CUTETUBE_DEBUG
    qDebug() << "Settings::setDownloadPath" << path;
#endif
}

int Settings::maximumConcurrentTransfers() const {
    return qBound(1, QSettings().value("Transfers/maximumConcurrentTransfers", 1).toInt(), MAX_CONCURRENT_TRANSFERS);
}

void Settings::setMaximumConcurrentTransfers(int maximum) {
    if (maximum != maximumConcurrentTransfers()) {
        QSettings().setValue("Transfers/maximumConcurrentTransfers", qBound(1, maximum, MAX_CONCURRENT_TRANSFERS));
        emit maximumConcurrentTransfersChanged();
    }
#ifdef CUTETUBE_DEBUG
    qDebug() << "Settings::setMaximumConcurrentTransfers" << maximum;
#endif
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
    return QSettings().value("Network/networkProxyEnabled", false).toBool();
}

void Settings::setNetworkProxyEnabled(bool enabled) {
    if (enabled != networkProxyEnabled()) {
        QSettings().setValue("Network/networkProxyEnabled", enabled);
        emit networkProxyChanged();
    }
#ifdef CUTETUBE_DEBUG
    qDebug() << "Settings::setNetworkProxyEnabled" << enabled;
#endif
}

QString Settings::networkProxyHost() const {
    return QSettings().value("Network/networkProxyHost").toString();
}

void Settings::setNetworkProxyHost(const QString &host) {
    if (host != networkProxyHost()) {
        QSettings().setValue("Network/networkProxyHost", host);
        emit networkProxyChanged();
    }
#ifdef CUTETUBE_DEBUG
    qDebug() << "Settings::setNetworkProxyHost" << host;
#endif
}

QString Settings::networkProxyPassword() const {
    return QByteArray::fromBase64(QSettings().value("Network/networkProxyPassword").toByteArray());
}

void Settings::setNetworkProxyPassword(const QString &password) {
    QByteArray pass = password.toUtf8().toBase64();
    
    if (pass != networkProxyPassword()) {
        QSettings().setValue("Network/networkProxyPassword", pass);
        emit networkProxyChanged();
    }
#ifdef CUTETUBE_DEBUG
    qDebug() << "Settings::setNetworkProxyPassword" << password;
#endif
}

int Settings::networkProxyPort() const {
    return QSettings().value("Network/networkProxyPort", 80).toInt();
}

void Settings::setNetworkProxyPort(int port) {
    if (port != networkProxyPort()) {
        QSettings().setValue("Network/networkProxyPort", port);
        emit networkProxyChanged();
    }
#ifdef CUTETUBE_DEBUG
    qDebug() << "Settings::setNetworkProxyPort" << port;
#endif
}

int Settings::networkProxyType() const {
    return QSettings().value("Network/networkProxyType", QNetworkProxy::ProxyType(QNetworkProxy::HttpProxy)).toInt();
}

void Settings::setNetworkProxyType(int type) {
    if (type != networkProxyType()) {
        QSettings().setValue("Network/networkProxyType", type);
        emit networkProxyChanged();
    }
#ifdef CUTETUBE_DEBUG
    qDebug() << "Settings::setNetworkProxyType" << type;
#endif
}

QString Settings::networkProxyUsername() const {
    return QSettings().value("Network/networkProxyUsername").toString();
}

void Settings::setNetworkProxyUsername(const QString &username) {
    if (username != networkProxyUsername()) {
        QSettings().setValue("Network/networkProxyUsername", username);
        emit networkProxyChanged();
    }
#ifdef CUTETUBE_DEBUG
    qDebug() << "Settings::setNetworkProxyUsername" << username;
#endif
}

bool Settings::safeSearchEnabled() const {
    return QSettings().value("Search/safeSearchEnabled", false).toBool();
}

void Settings::setSafeSearchEnabled(bool enabled) {
    if (enabled != safeSearchEnabled()) {
        QSettings().setValue("Search/safeSearchEnabled", enabled);
        emit safeSearchEnabledChanged();
    }
#ifdef CUTETUBE_DEBUG
    qDebug() << "Settings::setSafeSearchEnabled" << enabled;
#endif
}

bool Settings::startTransfersAutomatically() const {
    return QSettings().value("Transfers/startTransfersAutomatically", true).toBool();
}

void Settings::setStartTransfersAutomatically(bool enabled) {
    if (enabled != startTransfersAutomatically()) {
        QSettings().setValue("Transfers/startTransfersAutomatically", enabled);
        emit startTransfersAutomaticallyChanged();
    }
#ifdef CUTETUBE_DEBUG
    qDebug() << "Settings::setStartTransfersAutomatically" << enabled;
#endif
}

bool Settings::subtitlesEnabled() const {
    return QSettings().value("Content/subtitlesEnabled", false).toBool();
}

void Settings::setSubtitlesEnabled(bool enabled) {
    if (enabled != subtitlesEnabled()) {
        QSettings().setValue("Content/subtitlesEnabled", enabled);
        emit subtitlesEnabledChanged();
    }
#ifdef CUTETUBE_DEBUG
    qDebug() << "Settings::setSubtitlesEnabled" << enabled;
#endif
}

QString Settings::subtitlesLanguage() const {
    return QSettings().value("Content/subtitlesLanguage", "en").toString();
}

void Settings::setSubtitlesLanguage(const QString &language) {
    if (language != subtitlesLanguage()) {
        QSettings().setValue("Content/subtitlesLanguage", language);
        emit subtitlesLanguageChanged();
    }
#ifdef CUTETUBE_DEBUG
    qDebug() << "Settings::setSubtitlesLanguage" << language;
#endif
}

QString Settings::videoPlayer() const {
    return QSettings().value("Content/videoPlayer", "cutetube").toString().toLower();
}

void Settings::setVideoPlayer(const QString &player) {
    if (player != videoPlayer()) {
        QSettings().setValue("Content/videoPlayer", player.toLower());
        emit videoPlayerChanged();
    }
#ifdef CUTETUBE_DEBUG
    qDebug() << "Settings::setVideoPlayer" << player;
#endif
}

QString Settings::videoPlayerCommand() const {
    return QSettings().value("Content/videoPlayerCommand").toString();
}

void Settings::setVideoPlayerCommand(const QString &command) {
    if (command != videoPlayerCommand()) {
        QSettings().setValue("Content/videoPlayerCommand", command);
        emit videoPlayerCommandChanged();
    }
#ifdef CUTETUBE_DEBUG
    qDebug() << "Settings::setVideoPlayerCommand" << command;
#endif
}
