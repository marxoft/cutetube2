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

#include "rss.h"
#include <QCoreApplication>
#include <QStringList>
#include <QSettings>
#include <stdio.h>

int main(int argc, char *argv[]) {
    QCoreApplication app(argc, argv);
    Rss rss;

    QStringList args = app.arguments();
    QString method("list");
    QString resource("video");

    int i = 0;

    if ((i = args.indexOf("-m") + 1) > 0) {
        if (i < args.size()) {
            method = args.at(i);
        }
    }

    if ((i = args.indexOf("-r") + 1) > 0) {
        if (i < args.size()) {
            resource = args.at(i);
        }
    }

    if (resource != "video") {
        printf(qPrintable(QString("{\"error\": \"%1\"}").arg(QObject::tr("Resource '%1' is not supported")
                                                        .arg(resource))));
        return 1;
    }

    if (method == "list") {
        QString id;

        if ((i = args.indexOf("-i") + 1) > 0) {
            if (i < args.size()) {
                id = args.at(i);
            }
        }
        
        if (id.isEmpty()) {
            QStringList urls = QSettings("cuteTube2", "cuteTube2").value("RSS/feeds").toStringList();
            
            if (urls.isEmpty()) {
                printf(qPrintable(QString("{\"error\": \"%1\"}").arg(QObject::tr("No feed URLs specified"))));
                return 1;
            }
            else {
                rss.listVideos(urls);
            }
        }
        else {
            rss.listVideos(id);
        }
    }
    else {
        printf(qPrintable(QString("{\"error\": \"%1\"}").arg(QObject::tr("Method '%1' is not supported")
                                                        .arg(method))));
        return 1;
    }

    return app.exec();
}
