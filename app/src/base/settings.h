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

#ifndef SETTINGS_H
#define SETTINGS_H

#include <QObject>
#include <QStringList>
#include <QVariant>
#include <qplatformdefs.h>

struct Category {
    QString name;
    QString path;
};

class Settings : public QObject
{
    Q_OBJECT
    
#if (defined MEEGO_EDITION_HARMATTAN) || (SYMBIAN_OS)
    Q_PROPERTY(QString activeColor READ activeColor WRITE setActiveColor NOTIFY activeColorChanged)
    Q_PROPERTY(QString activeColorString READ activeColorString WRITE setActiveColorString
               NOTIFY activeColorStringChanged)
#endif
    Q_PROPERTY(QStringList categoryNames READ categoryNames NOTIFY categoriesChanged)
    Q_PROPERTY(QString defaultCategory READ defaultCategory WRITE setDefaultCategory NOTIFY defaultCategoryChanged)
    Q_PROPERTY(bool clipboardMonitorEnabled READ clipboardMonitorEnabled WRITE setClipboardMonitorEnabled
               NOTIFY clipboardMonitorEnabledChanged)
    Q_PROPERTY(QString currentService READ currentService WRITE setCurrentService NOTIFY currentServiceChanged)
    Q_PROPERTY(QString defaultViewMode READ defaultViewMode WRITE setDefaultViewMode NOTIFY defaultViewModeChanged)
    Q_PROPERTY(QString downloadPath READ downloadPath WRITE setDownloadPath NOTIFY downloadPathChanged)
    Q_PROPERTY(int maximumConcurrentTransfers READ maximumConcurrentTransfers WRITE setMaximumConcurrentTransfers
               NOTIFY maximumConcurrentTransfersChanged)
    Q_PROPERTY(bool networkProxyEnabled READ networkProxyEnabled WRITE setNetworkProxyEnabled
               NOTIFY networkProxyChanged)
    Q_PROPERTY(QString networkProxyHost READ networkProxyHost WRITE setNetworkProxyHost NOTIFY networkProxyChanged)
    Q_PROPERTY(QString networkProxyPassword READ networkProxyPassword WRITE setNetworkProxyPassword
               NOTIFY networkProxyChanged)
    Q_PROPERTY(int networkProxyPort READ networkProxyPort WRITE setNetworkProxyPort NOTIFY networkProxyChanged)
    Q_PROPERTY(int networkProxyType READ networkProxyType WRITE setNetworkProxyType NOTIFY networkProxyChanged)
    Q_PROPERTY(QString networkProxyUsername READ networkProxyUsername WRITE setNetworkProxyUsername
               NOTIFY networkProxyChanged)
    Q_PROPERTY(bool safeSearchEnabled READ safeSearchEnabled WRITE setSafeSearchEnabled NOTIFY safeSearchEnabledChanged)
    Q_PROPERTY(int screenOrientation READ screenOrientation WRITE setScreenOrientation NOTIFY screenOrientationChanged)
    Q_PROPERTY(QStringList searchHistory READ searchHistory WRITE setSearchHistory NOTIFY searchHistoryChanged)
    Q_PROPERTY(bool startTransfersAutomatically READ startTransfersAutomatically WRITE setStartTransfersAutomatically
               NOTIFY startTransfersAutomaticallyChanged)
    Q_PROPERTY(bool subtitlesEnabled READ subtitlesEnabled WRITE setSubtitlesEnabled NOTIFY subtitlesEnabledChanged)
    Q_PROPERTY(QString subtitlesLanguage READ subtitlesLanguage WRITE setSubtitlesLanguage
               NOTIFY subtitlesLanguageChanged)
    Q_PROPERTY(QString videoPlayer READ videoPlayer WRITE setVideoPlayer NOTIFY videoPlayerChanged)
    Q_PROPERTY(QString videoPlayerCommand READ videoPlayerCommand WRITE setVideoPlayerCommand
               NOTIFY videoPlayerCommandChanged)
    
public:
    explicit Settings(QObject *parent = 0);
    ~Settings();
    
    static Settings* instance();
    
#if (defined MEEGO_EDITION_HARMATTAN) || (SYMBIAN_OS)
    QString activeColor() const;
    QString activeColorString() const;
#endif
    
    QStringList categoryNames() const;
    QList<Category> categories() const;
    void setCategories(const QList<Category> &c);
    
    bool clipboardMonitorEnabled() const;
    
    QString currentService() const;
    
    QString defaultCategory() const;
    
    Q_INVOKABLE QString defaultDownloadFormat(const QString &service) const;
    Q_INVOKABLE QString defaultPlaybackFormat(const QString &service) const;
    
    Q_INVOKABLE QString defaultSearchOrder(const QString &service) const;
    Q_INVOKABLE QString defaultSearchType(const QString &service) const;
    
    QString defaultViewMode() const;
    
    QString downloadPath() const;
    Q_INVOKABLE QString downloadPath(const QString &category) const;
        
    int maximumConcurrentTransfers() const;
    
    bool networkProxyEnabled() const;
    QString networkProxyHost() const;
    QString networkProxyPassword() const;
    int networkProxyPort() const;
    int networkProxyType() const;
    QString networkProxyUsername() const;
    
    bool safeSearchEnabled() const;
    
    int screenOrientation() const;
    
    QStringList searchHistory() const;
    void setSearchHistory(const QStringList &searches);
    
    bool startTransfersAutomatically() const;
    
    bool subtitlesEnabled() const;
    QString subtitlesLanguage() const;

    Q_INVOKABLE QVariant value(const QString &key, const QVariant &defaultValue = QVariant()) const;
    
    QString videoPlayer() const;
    QString videoPlayerCommand() const;

public Q_SLOTS:
#if (defined MEEGO_EDITION_HARMATTAN) || (SYMBIAN_OS)
    void setActiveColor(const QString &color);
    void setActiveColorString(const QString &s);
#endif
    void addCategory(const QString &name, const QString &path);
    void setDefaultCategory(const QString &category);
    void removeCategory(const QString &name);
    
    void setClipboardMonitorEnabled(bool enabled);
    
    void setCurrentService(const QString &service);
    
    void setDefaultDownloadFormat(const QString &service, const QString &format);
    void setDefaultPlaybackFormat(const QString &service, const QString &format);
    
    void setDefaultSearchOrder(const QString &service, const QString &order);
    void setDefaultSearchType(const QString &service, const QString &type);
    
    void setDefaultViewMode(const QString &mode);
    
    void setDownloadPath(const QString &path);
    
    void setMaximumConcurrentTransfers(int maximum);
    
    void setNetworkProxy();
    void setNetworkProxyEnabled(bool enabled);
    void setNetworkProxyHost(const QString &host);
    void setNetworkProxyPassword(const QString &password);
    void setNetworkProxyPort(int port);
    void setNetworkProxyType(int type);
    void setNetworkProxyUsername(const QString &username);
    
    void setSafeSearchEnabled(bool enabled);
    
    void setScreenOrientation(int orientation);
    
    void addSearch(const QString &query);
    void removeSearch(const QString &query);
    
    void setStartTransfersAutomatically(bool enabled);
    
    void setSubtitlesEnabled(bool enabled);
    void setSubtitlesLanguage(const QString &language);

    void setValue(const QString &key, const QVariant &value);
    
    void setVideoPlayer(const QString &player);
    void setVideoPlayerCommand(const QString &command);

Q_SIGNALS:
#if (defined MEEGO_EDITION_HARMATTAN) || (SYMBIAN_OS)
    void activeColorChanged();
    void activeColorStringChanged();
#endif
    void categoriesChanged();
    void defaultCategoryChanged();
    void clipboardMonitorEnabledChanged();
    void currentServiceChanged();
    void defaultSearchOrderChanged();
    void defaultSearchTypeChanged();
    void defaultViewModeChanged();
    void downloadFormatsChanged();
    void downloadPathChanged();
    void maximumConcurrentTransfersChanged();
    void networkProxyChanged();
    void playbackFormatsChanged();
    void safeSearchEnabledChanged();
    void screenOrientationChanged();
    void searchHistoryChanged();
    void startTransfersAutomaticallyChanged();
    void subtitlesEnabledChanged();
    void subtitlesLanguageChanged();
    void videoPlayerChanged();
    void videoPlayerCommandChanged();

private:
    static Settings *self;
};
    
#endif // SETTINGS_H
