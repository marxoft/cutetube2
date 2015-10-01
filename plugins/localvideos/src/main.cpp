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

#include "galleryrequest.h"
#include <QCoreApplication>
#include <QStringList>
#include <iostream>

int main(int argc, char *argv[]) {
    QCoreApplication app(argc, argv);
    GalleryRequest request;
    QObject::connect(&request, SIGNAL(finished()), &app, SLOT(quit()));

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
        std::cout << qPrintable(QString("{\"error\": \"%1\"}").arg(QObject::tr("Resource '%1' is not supported")
                                                              .arg(resource)));
        return 1;
    }

    if ((method == "list") || (method == "get")) {
        QString id;

        if ((i = args.indexOf("-i") + 1) > 0) {
            if (i < args.size()) {
                id = args.at(i);
            }
        }
        
        if (method == "list") {
            request.listVideos(id);
        }
        else {
            request.getVideo(id);
        }
    }
    else if (method == "search") {
        QString query;
        QString order;

        if ((i = args.indexOf("-q") + 1) > 0) {
            if (i < args.size()) {
                query = args.at(i);
            }
        }

        if ((i = args.indexOf("-o") + 1) > 0) {
            if (i < args.size()) {
                order = args.at(i);
            }
        }

        request.searchVideos(query, order);
    }
    else {
        std::cout << qPrintable(QString("{\"error\": \"%1\"}").arg(QObject::tr("Method '%1' is not supported")
                                                              .arg(method)));
        return 1;
    }

    return app.exec();
}
