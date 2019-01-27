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

#include "activecolormodel.h"
#include "categorymodel.h"
#include "categorynamemodel.h"
#include "clipboard.h"
#include "concurrenttransfersmodel.h"
#include "cookiejar.h"
#include "dailymotion.h"
#include "dailymotionaccountmodel.h"
#include "dailymotioncategorymodel.h"
#include "dailymotioncommentmodel.h"
#include "dailymotionnavmodel.h"
#include "dailymotionplaylistmodel.h"
#include "dailymotionsearchtypemodel.h"
#include "dailymotionstreammodel.h"
#include "dailymotionsubtitlemodel.h"
#include "dailymotionusermodel.h"
#include "dailymotionvideomodel.h"
#include "database.h"
#include "dbusservice.h"
#include "definitions.h"
#include "localemodel.h"
#include "logger.h"
#include "maskeditem.h"
//#include "mediakeycaptureitem.h"
#include "networkaccessmanagerfactory.h"
#include "networkproxytypemodel.h"
#include "plugincategorymodel.h"
#include "plugincommentmodel.h"
#include "pluginconfigmodel.h"
#include "pluginmanager.h"
#include "pluginnavmodel.h"
#include "pluginplaylistmodel.h"
#include "pluginsearchtypemodel.h"
#include "pluginsettings.h"
#include "pluginstreammodel.h"
#include "pluginsubtitlemodel.h"
#include "pluginusermodel.h"
#include "pluginvideomodel.h"
#include "resources.h"
#include "screenorientationmodel.h"
#include "screensaver.h"
#include "searchhistorymodel.h"
#include "servicemodel.h"
#include "settings.h"
#include "shareui.h"
#include "transfers.h"
#include "utils.h"
#include "videomodel.h"
#include "videolauncher.h"
#include "videoplayermodel.h"
#include "vimeo.h"
#include "vimeoaccountmodel.h"
#include "vimeocategorymodel.h"
#include "vimeocommentmodel.h"
#include "vimeonavmodel.h"
#include "vimeoplaylistmodel.h"
#include "vimeosearchtypemodel.h"
#include "vimeostreammodel.h"
#include "vimeosubtitlemodel.h"
#include "vimeousermodel.h"
#include "vimeovideomodel.h"
#include "youtube.h"
#include "youtubeaccountmodel.h"
#include "youtubecategorymodel.h"
#include "youtubecommentmodel.h"
#include "youtubenavmodel.h"
#include "youtubeplaylistmodel.h"
#include "youtubesearchtypemodel.h"
#include "youtubestreammodel.h"
#include "youtubesubtitlemodel.h"
#include "youtubeusermodel.h"
#include "youtubevideomodel.h"
#include <qdailymotion/authenticationrequest.h>
#include <qdailymotion/resourcesrequest.h>
#include <qdailymotion/streamsrequest.h>
#include <qvimeo/authenticationrequest.h>
#include <qvimeo/resourcesrequest.h>
#include <qvimeo/streamsrequest.h>
#include <qyoutube/authenticationrequest.h>
#include <qyoutube/resourcesrequest.h>
#include <qyoutube/streamsrequest.h>
#include <qyoutube/subtitlesrequest.h>
#include <QApplication>
#include <QDeclarativeView>
#include <QDeclarativeContext>
#include <QDeclarativeEngine>
#include <qdeclarative.h>
#include <QGLWidget>
#include <QSsl>
#include <QSslConfiguration>

inline void registerTypes() {
    qmlRegisterType<ActiveColorModel>("cuteTube", 2, 0, "ActiveColorModel");
    qmlRegisterType<CategoryModel>("cuteTube", 2, 0, "CategoryModel");
    qmlRegisterType<CategoryNameModel>("cuteTube", 2, 0, "CategoryNameModel");
    qmlRegisterType<ConcurrentTransfersModel>("cuteTube", 2, 0, "ConcurrentTransfersModel");
    qmlRegisterType<DailymotionAccountModel>("cuteTube", 2, 0, "DailymotionAccountModel");
    qmlRegisterType<DailymotionCategoryModel>("cuteTube", 2, 0, "DailymotionCategoryModel");
    qmlRegisterType<DailymotionComment>("cuteTube", 2, 0, "DailymotionComment");
    qmlRegisterType<DailymotionCommentModel>("cuteTube", 2, 0, "DailymotionCommentModel");
    qmlRegisterType<DailymotionNavModel>("cuteTube", 2, 0, "DailymotionNavModel");
    qmlRegisterType<DailymotionPlaylist>("cuteTube", 2, 0, "DailymotionPlaylist");
    qmlRegisterType<DailymotionPlaylistModel>("cuteTube", 2, 0, "DailymotionPlaylistModel");
    qmlRegisterType<DailymotionSearchTypeModel>("cuteTube", 2, 0, "DailymotionSearchTypeModel");
    qmlRegisterType<DailymotionStreamModel>("cuteTube", 2, 0, "DailymotionStreamModel");
    qmlRegisterType<DailymotionSubtitleModel>("cuteTube", 2, 0, "DailymotionSubtitleModel");
    qmlRegisterType<DailymotionUser>("cuteTube", 2, 0, "DailymotionUser");
    qmlRegisterType<DailymotionUserModel>("cuteTube", 2, 0, "DailymotionUserModel");
    qmlRegisterType<DailymotionVideo>("cuteTube", 2, 0, "DailymotionVideo");
    qmlRegisterType<DailymotionVideoModel>("cuteTube", 2, 0, "DailymotionVideoModel");
    qmlRegisterType<LocaleModel>("cuteTube", 2, 0, "LocaleModel");
    qmlRegisterType<MaskedItem>("cuteTube", 2, 0, "MaskedItem");
    qmlRegisterType<NetworkProxyTypeModel>("cuteTube", 2, 0, "NetworkProxyTypeModel");
    qmlRegisterType<PluginCategoryModel>("cuteTube", 2, 0, "PluginCategoryModel");
    qmlRegisterType<PluginComment>("cuteTube", 2, 0, "PluginComment");
    qmlRegisterType<PluginCommentModel>("cuteTube", 2, 0, "PluginCommentModel");
    qmlRegisterType<PluginConfigModel>("cuteTube", 2, 0, "PluginConfigModel");
    qmlRegisterType<PluginNavModel>("cuteTube", 2, 0, "PluginNavModel");
    qmlRegisterType<PluginPlaylist>("cuteTube", 2, 0, "PluginPlaylist");
    qmlRegisterType<PluginPlaylistModel>("cuteTube", 2, 0, "PluginPlaylistModel");
    qmlRegisterType<PluginSearchTypeModel>("cuteTube", 2, 0, "PluginSearchTypeModel");
    qmlRegisterType<PluginSettings>("cuteTube", 2, 0, "PluginSettings");
    qmlRegisterType<PluginStreamModel>("cuteTube", 2, 0, "PluginStreamModel");
    qmlRegisterType<PluginSubtitleModel>("cuteTube", 2, 0, "PluginSubtitleModel");
    qmlRegisterType<PluginUser>("cuteTube", 2, 0, "PluginUser");
    qmlRegisterType<PluginUserModel>("cuteTube", 2, 0, "PluginUserModel");
    qmlRegisterType<PluginVideo>("cuteTube", 2, 0, "PluginVideo");
    qmlRegisterType<PluginVideoModel>("cuteTube", 2, 0, "PluginVideoModel");
    qmlRegisterType<ScreenOrientationModel>("cuteTube", 2, 0, "ScreenOrientationModel");
    qmlRegisterType<ScreenSaver>("cuteTube", 2, 0, "ScreenSaver");
    qmlRegisterType<SearchHistoryModel>("cuteTube", 2, 0, "SearchHistoryModel");
    qmlRegisterType<SelectionModel>("cuteTube", 2, 0, "SelectionModel");
    qmlRegisterType<ServiceModel>("cuteTube", 2, 0, "ServiceModel");
    qmlRegisterType<CTVideo>("cuteTube", 2, 0, "Video");
    qmlRegisterType<VideoModel>("cuteTube", 2, 0, "VideoModel");
    qmlRegisterType<VideoPlayerModel>("cuteTube", 2, 0, "VideoPlayerModel");
    qmlRegisterType<VimeoAccountModel>("cuteTube", 2, 0, "VimeoAccountModel");
    qmlRegisterType<VimeoCategoryModel>("cuteTube", 2, 0, "VimeoCategoryModel");
    qmlRegisterType<VimeoComment>("cuteTube", 2, 0, "VimeoComment");
    qmlRegisterType<VimeoCommentModel>("cuteTube", 2, 0, "VimeoCommentModel");
    qmlRegisterType<VimeoNavModel>("cuteTube", 2, 0, "VimeoNavModel");
    qmlRegisterType<VimeoPlaylist>("cuteTube", 2, 0, "VimeoPlaylist");
    qmlRegisterType<VimeoPlaylistModel>("cuteTube", 2, 0, "VimeoPlaylistModel");
    qmlRegisterType<VimeoSearchTypeModel>("cuteTube", 2, 0, "VimeoSearchTypeModel");
    qmlRegisterType<VimeoStreamModel>("cuteTube", 2, 0, "VimeoStreamModel");
    qmlRegisterType<VimeoSubtitleModel>("cuteTube", 2, 0, "VimeoSubtitleModel");
    qmlRegisterType<VimeoUser>("cuteTube", 2, 0, "VimeoUser");
    qmlRegisterType<VimeoUserModel>("cuteTube", 2, 0, "VimeoUserModel");
    qmlRegisterType<VimeoVideo>("cuteTube", 2, 0, "VimeoVideo");
    qmlRegisterType<VimeoVideoModel>("cuteTube", 2, 0, "VimeoVideoModel");
    qmlRegisterType<YouTubeAccountModel>("cuteTube", 2, 0, "YouTubeAccountModel");
    qmlRegisterType<YouTubeCategoryModel>("cuteTube", 2, 0, "YouTubeCategoryModel");
    qmlRegisterType<YouTubeComment>("cuteTube", 2, 0, "YouTubeComment");
    qmlRegisterType<YouTubeCommentModel>("cuteTube", 2, 0, "YouTubeCommentModel");
    qmlRegisterType<YouTubeNavModel>("cuteTube", 2, 0, "YouTubeNavModel");
    qmlRegisterType<YouTubePlaylist>("cuteTube", 2, 0, "YouTubePlaylist");
    qmlRegisterType<YouTubePlaylistModel>("cuteTube", 2, 0, "YouTubePlaylistModel");
    qmlRegisterType<YouTubeSearchTypeModel>("cuteTube", 2, 0, "YouTubeSearchTypeModel");
    qmlRegisterType<YouTubeStreamModel>("cuteTube", 2, 0, "YouTubeStreamModel");
    qmlRegisterType<YouTubeSubtitleModel>("cuteTube", 2, 0, "YouTubeSubtitleModel");
    qmlRegisterType<YouTubeUser>("cuteTube", 2, 0, "YouTubeUser");
    qmlRegisterType<YouTubeUserModel>("cuteTube", 2, 0, "YouTubeUserModel");
    qmlRegisterType<YouTubeVideo>("cuteTube", 2, 0, "YouTubeVideo");
    qmlRegisterType<YouTubeVideoModel>("cuteTube", 2, 0, "YouTubeVideoModel");

    qmlRegisterType<QDailymotion::AuthenticationRequest>("QDailymotion", 1, 0, "AuthenticationRequest");
    qmlRegisterType<QDailymotion::ResourcesRequest>("QDailymotion", 1, 0, "ResourcesRequest");
    qmlRegisterType<QDailymotion::StreamsRequest>("QDailymotion", 1, 0, "StreamsRequest");

    qmlRegisterType<QVimeo::AuthenticationRequest>("QVimeo", 1, 0, "AuthenticationRequest");
    qmlRegisterType<QVimeo::ResourcesRequest>("QVimeo", 1, 0, "ResourcesRequest");
    qmlRegisterType<QVimeo::StreamsRequest>("QVimeo", 1, 0, "StreamsRequest");

    qmlRegisterType<QYouTube::AuthenticationRequest>("QYouTube", 1, 0, "AuthenticationRequest");
    qmlRegisterType<QYouTube::ResourcesRequest>("QYouTube", 1, 0, "ResourcesRequest");
    qmlRegisterType<QYouTube::StreamsRequest>("QYouTube", 1, 0, "StreamsRequest");
    qmlRegisterType<QYouTube::SubtitlesRequest>("QYouTube", 1, 0, "SubtitlesRequest");
    
    qmlRegisterUncreatableType<ResourcesRequest>("cuteTube", 2, 0, "ResourcesRequest", "");
    qmlRegisterUncreatableType<Transfer>("cuteTube", 2, 0, "Transfer", "");

    qRegisterMetaType<ResourcesRequest::Status>("ResourcesRequest::Status");
}

Q_DECL_EXPORT int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    app.setOrganizationName("cuteTube2");
    app.setApplicationName("cuteTube2");

    const QStringList args = app.arguments();
    const int verbosity = args.indexOf("-v") + 1;
    
    if ((verbosity > 1) && (verbosity < args.size())) {
        Logger::setVerbosity(qMax(1, args.at(verbosity).toInt()));
    }
    else {
        Logger::setFileName(Settings::loggerFileName());
        Logger::setVerbosity(Settings::loggerVerbosity());
    }
    
    QSslConfiguration config = QSslConfiguration::defaultConfiguration();
    config.setProtocol(QSsl::TlsV1);
    QSslConfiguration::setDefaultConfiguration(config);
    
    QScopedPointer<Settings> settings(Settings::instance());
    QScopedPointer<Clipboard> clipboard(Clipboard::instance());
    QScopedPointer<Dailymotion> dailymotion(Dailymotion::instance());
    QScopedPointer<PluginManager> plugins(PluginManager::instance());
    QScopedPointer<Transfers> transfers(Transfers::instance());
    QScopedPointer<Vimeo> vimeo(Vimeo::instance());
    QScopedPointer<YouTube> youtube(YouTube::instance());
    
    DBusService dbus;
    Logger logger;
    NetworkAccessManagerFactory factory;
    Resources resources;
    ShareUi shareui;
    Utils utils;
    VideoLauncher launcher;
    
    initDatabase();
    registerTypes();
    Settings::setNetworkProxy();
    Dailymotion::init();
    Vimeo::init();
    YouTube::init();
    
    QDeclarativeView view;
    QDeclarativeContext *context = view.rootContext();
    
    context->setContextProperty("Clipboard", clipboard.data());
    context->setContextProperty("CookieJar", factory.cookieJar());
    context->setContextProperty("Dailymotion", dailymotion.data());
    context->setContextProperty("DBus", &dbus);
    context->setContextProperty("Logger", &logger);
    context->setContextProperty("MainWindow", &view);
    context->setContextProperty("Plugins", plugins.data());
    context->setContextProperty("Resources", &resources);
    context->setContextProperty("Settings", settings.data());
    context->setContextProperty("ShareUi", &shareui);
    context->setContextProperty("Transfers", transfers.data());
    context->setContextProperty("Utils", &utils);
    context->setContextProperty("VideoLauncher", &launcher);
    context->setContextProperty("Vimeo", vimeo.data());
    context->setContextProperty("YouTube", youtube.data());
    context->setContextProperty("MAX_RESULTS", MAX_RESULTS);
    context->setContextProperty("VERSION_NUMBER", VERSION_NUMBER);

    view.engine()->setNetworkAccessManagerFactory(&factory);
    
    view.setViewport(new QGLWidget);
    view.setSource(QUrl::fromLocalFile("/opt/cutetube2/qml/main.qml"));
    view.showFullScreen();
    
    return app.exec();
}
