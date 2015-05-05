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

#include "clipboardmonitor.h"
#include "database.h"
#include "dailymotion.h"
#include "dbusservice.h"
#include "mainwindow.h"
#include "resourcesplugins.h"
#include "settings.h"
#include "transfers.h"
#include "vimeo.h"
#include "youtube.h"
#include <QApplication>
#include <QSsl>
#include <QSslConfiguration>

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    app.setOrganizationName("cuteTube2");
    app.setApplicationName("cuteTube2");
    
    QSslConfiguration config = QSslConfiguration::defaultConfiguration();
    config.setProtocol(QSsl::TlsV1);
    QSslConfiguration::setDefaultConfiguration(config);

    Settings settings;
    ClipboardMonitor clipboard;
    Dailymotion dailymotion;
    DBusService dbus;
    ResourcesPlugins plugins;
    Transfers transfers;
    Vimeo vimeo;
    YouTube youtube;
    
    initDatabase();
    plugins.load();
    settings.setNetworkProxy();
    transfers.restoreTransfers();
    
    MainWindow window;
    window.show();
    
    return app.exec();
}
