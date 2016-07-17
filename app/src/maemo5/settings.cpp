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

#include "settings.h"
#include "definitions.h"
#include "resources.h"
#include <QSettings>
#include <QNetworkProxy>

Settings* Settings::self = 0;

Settings::Settings() :
    QObject()
{
}

Settings::~Settings() {
    self = 0;
}

Settings* Settings::instance() {
    return self ? self : self = new Settings;
}

QStringList Settings::categoryNames() {
    QSettings settings;
    settings.beginGroup("Categories");
    QStringList names = settings.childKeys();
    names.prepend(tr("Default"));
    settings.endGroup();
    
    return names;
}

QList<Category> Settings::categories() {
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

    if (self) {
        emit self->categoriesChanged();
    }
}

void Settings::addCategory(const QString &name, const QString &path) {
    if (path != downloadPath(name)) {
        setValue("Categories/" + name, path);

        if (self) {
            emit self->categoriesChanged();
        }
    }
}

void Settings::removeCategory(const QString &name) {
    QSettings settings;
    settings.beginGroup("Categories");
    
    if (settings.contains(name)) {
        settings.remove(name);

        if (self) {
            emit self->categoriesChanged();
        }
    }
    
    settings.endGroup();
}

QString Settings::defaultCategory() {
    return value("Transfers/defaultCategory", tr("Default")).toString();
}

void Settings::setDefaultCategory(const QString &category) {
    if (category != defaultCategory()) {
        setValue("Transfers/defaultCategory", category);

        if (self) {
            emit self->defaultCategoryChanged(category);
        }
    }
}

bool Settings::clipboardMonitorEnabled() {
    return value("Content/clipboardMonitorEnabled", false).toBool();
}

void Settings::setClipboardMonitorEnabled(bool enabled) {
    if (enabled != clipboardMonitorEnabled()) {
        setValue("Content/clipboardMonitorEnabled", enabled);

        if (self) {
            emit self->clipboardMonitorEnabledChanged(enabled);
        }
    }
}

QString Settings::currentService() {
    return value("Content/currentService", Resources::YOUTUBE).toString();
}

void Settings::setCurrentService(const QString &service) {
    if (service != currentService()) {
        setValue("Content/currentService", service);

        if (self) {
            emit self->currentServiceChanged(service);
        }
    }
}

QString Settings::customTransferCommand() {
    return value("Transfers/customCommand").toString();
}

void Settings::setCustomTransferCommand(const QString &command) {
    if (command != customTransferCommand()) {
        setValue("Transfers/customCommand", command);

        if (self) {
            emit self->customTransferCommandChanged(command);
        }
    }
}

bool Settings::customTransferCommandEnabled() {
    return value("Transfers/customCommandEnabled", false).toBool();
}

void Settings::setCustomTransferCommandEnabled(bool enabled) {
    if (enabled != customTransferCommandEnabled()) {
        setValue("Transfers/customCommandEnabled", enabled);
        
        if (self) {
            emit self->customTransferCommandEnabledChanged(enabled);
        }
    }
}

QString Settings::defaultDownloadFormat(const QString &service) {
    return value("DownloadFormats/" + service).toString();
}

void Settings::setDefaultDownloadFormat(const QString &service, const QString &format) {
    if (format != defaultDownloadFormat(service)) {
        setValue("DownloadFormats/" + service, format);

        if (self) {
            emit self->downloadFormatsChanged();
        }
    }
}

QString Settings::defaultPlaybackFormat(const QString &service) {
    return value("PlaybackFormats/" + service).toString();
}

void Settings::setDefaultPlaybackFormat(const QString &service, const QString &format) {
    if (format != defaultPlaybackFormat(service)) {
        setValue("PlaybackFormats/" + service, format);

        if (self) {
            emit self->playbackFormatsChanged();
        }
    }
}

QString Settings::defaultSearchType(const QString &service) {
    return value("Search/searchType/" + service).toString();
}

void Settings::setDefaultSearchType(const QString &service, const QString &type) {
    if (type != defaultSearchType(service)) {
        setValue("Search/searchType/" + service, type);

        if (self) {
            emit self->defaultSearchTypeChanged();
        }
    }
}

QString Settings::downloadPath() {
    QString path = value("Transfers/downloadPath", DOWNLOAD_PATH).toString();

    if (!path.endsWith("/")) {
        path.append("/");
    }
    
    return path;
}

QString Settings::downloadPath(const QString &category) {
    return value("Categories/" + category, downloadPath()).toString();
}   

void Settings::setDownloadPath(const QString &path) {
    if (path != downloadPath()) {
        setValue("Transfers/downloadPath", path);

        if (self) {
            emit self->downloadPathChanged(path);
        }
    }
}

QString Settings::locale() {
    return value("Content/locale", QLocale().name()).toString();
}

void Settings::setLocale(const QString &name) {
    if (name != locale()) {
        setValue("Content/locale", name);

        if (self) {
            emit self->localeChanged(name);
        }
    }
}

int Settings::maximumConcurrentTransfers() {
    return qBound(1, value("Transfers/maximumConcurrentTransfers", 1).toInt(), MAX_CONCURRENT_TRANSFERS);
}

void Settings::setMaximumConcurrentTransfers(int maximum) {
    if (maximum != maximumConcurrentTransfers()) {
        maximum = qBound(1, maximum, MAX_CONCURRENT_TRANSFERS);
        setValue("Transfers/maximumConcurrentTransfers", maximum);

        if (self) {
            emit self->maximumConcurrentTransfersChanged(maximum);
        }
    }
}

void Settings::setNetworkProxy() {
    if (networkProxyEnabled()) {
        QNetworkProxy::setApplicationProxy(QNetworkProxy(QNetworkProxy::ProxyType(networkProxyType()),
                                           networkProxyHost(), networkProxyPort(), networkProxyUsername(),
                                           networkProxyPassword()));
    }
    else {
        QNetworkProxy::setApplicationProxy(QNetworkProxy());
    }
}

bool Settings::networkProxyEnabled() {
    return value("Network/networkProxyEnabled", false).toBool();
}

void Settings::setNetworkProxyEnabled(bool enabled) {
    if (enabled != networkProxyEnabled()) {
        setValue("Network/networkProxyEnabled", enabled);

        if (self) {
            emit self->networkProxyChanged();
        }
    }
}

QString Settings::networkProxyHost() {
    return value("Network/networkProxyHost").toString();
}

void Settings::setNetworkProxyHost(const QString &host) {
    if (host != networkProxyHost()) {
        setValue("Network/networkProxyHost", host);

        if (self) {
            emit self->networkProxyChanged();
        }
    }
}

QString Settings::networkProxyPassword() {
    return QByteArray::fromBase64(value("Network/networkProxyPassword").toByteArray());
}

void Settings::setNetworkProxyPassword(const QString &password) {
    QByteArray pass = password.toUtf8().toBase64();
    
    if (pass != networkProxyPassword()) {
        setValue("Network/networkProxyPassword", pass);

        if (self) {
            emit self->networkProxyChanged();
        }
    }
}

int Settings::networkProxyPort() {
    return value("Network/networkProxyPort", 80).toInt();
}

void Settings::setNetworkProxyPort(int port) {
    if (port != networkProxyPort()) {
        setValue("Network/networkProxyPort", port);

        if (self) {
            emit self->networkProxyChanged();
        }
    }
}

int Settings::networkProxyType() {
    return value("Network/networkProxyType", QNetworkProxy::ProxyType(QNetworkProxy::HttpProxy)).toInt();
}

void Settings::setNetworkProxyType(int type) {
    if (type != networkProxyType()) {
        setValue("Network/networkProxyType", type);

        if (self) {
            emit self->networkProxyChanged();
        }
    }
}

QString Settings::networkProxyUsername() {
    return value("Network/networkProxyUsername").toString();
}

void Settings::setNetworkProxyUsername(const QString &username) {
    if (username != networkProxyUsername()) {
        setValue("Network/networkProxyUsername", username);

        if (self) {
            emit self->networkProxyChanged();
        }
    }
}

bool Settings::safeSearchEnabled() {
    return value("Search/safeSearchEnabled", false).toBool();
}

void Settings::setSafeSearchEnabled(bool enabled) {
    if (enabled != safeSearchEnabled()) {
        setValue("Search/safeSearchEnabled", enabled);

        if (self) {
            emit self->safeSearchEnabledChanged(enabled);
        }
    }
}

QStringList Settings::searchHistory() {
    return value("Search/searchHistory").toStringList();
}

void Settings::setSearchHistory(const QStringList &searches) {
    setValue("Search/searchHistory", searches);

    if (self) {
        emit self->searchHistoryChanged();
    }
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

bool Settings::startTransfersAutomatically() {
    return value("Transfers/startTransfersAutomatically", true).toBool();
}

void Settings::setStartTransfersAutomatically(bool enabled) {
    if (enabled != startTransfersAutomatically()) {
        setValue("Transfers/startTransfersAutomatically", enabled);

        if (self) {
            emit self->startTransfersAutomaticallyChanged(enabled);
        }
    }
}

bool Settings::subtitlesEnabled() {
    return value("Content/subtitlesEnabled", false).toBool();
}

void Settings::setSubtitlesEnabled(bool enabled) {
    if (enabled != subtitlesEnabled()) {
        setValue("Content/subtitlesEnabled", enabled);

        if (self) {
            emit self->subtitlesEnabledChanged(enabled);
        }
    }
}

QString Settings::subtitlesLanguage() {
    return value("Content/subtitlesLanguage").toString();
}

void Settings::setSubtitlesLanguage(const QString &language) {
    if (language != subtitlesLanguage()) {
        setValue("Content/subtitlesLanguage", language);

        if (self) {
            emit self->subtitlesLanguageChanged(language);
        }
    }
}

QByteArray Settings::transfersHeaderViewState() {
    return value("UI/transfersHeaderViewState").toByteArray();
}

void Settings::setTransfersHeaderViewState(const QByteArray &state) {
    setValue("UI/transfersHeaderViewState", state);
}

QVariant Settings::value(const QString &key, const QVariant &defaultValue) {
    return QSettings().value(key, defaultValue);
}

void Settings::setValue(const QString &key, const QVariant &value) {
    QSettings().setValue(key, value);
}

QString Settings::videoPlayer() {
    return value("Content/videoPlayer", "cutetube").toString().toLower();
}

void Settings::setVideoPlayer(const QString &player) {
    if (player != videoPlayer()) {
        setValue("Content/videoPlayer", player.toLower());

        if (self) {
            emit self->videoPlayerChanged(player.toLower());
        }
    }
}

QString Settings::videoPlayerCommand() {
    return value("Content/videoPlayerCommand").toString();
}

void Settings::setVideoPlayerCommand(const QString &command) {
    if (command != videoPlayerCommand()) {
        setValue("Content/videoPlayerCommand", command);

        if (self) {
            emit self->videoPlayerCommandChanged(command);
        }
    }
}
