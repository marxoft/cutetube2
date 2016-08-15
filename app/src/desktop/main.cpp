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

#include "clipboard.h"
#include "dailymotion.h"
#include "database.h"
#include "dbusservice.h"
#include "definitions.h"
#include "logger.h"
#include "mainwindow.h"
#include "pluginmanager.h"
#include "settings.h"
#include "transfers.h"
#include "vimeo.h"
#include "youtube.h"
#include <QApplication>

Q_DECL_EXPORT int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    app.setOrganizationName("cuteTube2");
    app.setApplicationName("cuteTube2");
    app.setApplicationVersion(VERSION_NUMBER);
    
    const QStringList args = app.arguments();
    const int verbosity = args.indexOf("-v") + 1;
    
    if ((verbosity > 1) && (verbosity < args.size())) {
        Logger::setVerbosity(qMax(1, args.at(verbosity).toInt()));
    }
    else {
        Logger::setFileName(Settings::loggerFileName());
        Logger::setVerbosity(Settings::loggerVerbosity());
    }
    
    QScopedPointer<Settings> settings(Settings::instance());
    QScopedPointer<Clipboard> clipboard(Clipboard::instance());
    QScopedPointer<Dailymotion> dailymotion(Dailymotion::instance());
    QScopedPointer<PluginManager> plugins(PluginManager::instance());
    QScopedPointer<Transfers> transfers(Transfers::instance());
    QScopedPointer<Vimeo> vimeo(Vimeo::instance());
    QScopedPointer<YouTube> youtube(YouTube::instance());
    DBusService dbus;    
    
    initDatabase();
    Settings::setNetworkProxy();
    Dailymotion::init();
    Vimeo::init();
    YouTube::init();

    clipboard.data()->setEnabled(Settings::clipboardMonitorEnabled());
    plugins.data()->load();
    transfers.data()->restore();
    
    MainWindow window;
    window.show();

    QObject::connect(&app, SIGNAL(aboutToQuit()), transfers.data(), SLOT(save()));
    QObject::connect(settings.data(), SIGNAL(clipboardMonitorEnabledChanged(bool)),
                     clipboard.data(), SLOT(setEnabled(bool)));
    
    return app.exec();
}
