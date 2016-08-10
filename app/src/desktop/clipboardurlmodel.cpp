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

#include "clipboardurlmodel.h"
#include "definitions.h"
#include "logger.h"
#include "resources.h"
#include <QApplication>
#include <QClipboard>
#include <QFile>
#include <QTextStream>

ClipboardUrlModel* ClipboardUrlModel::self = 0;

ClipboardUrlModel::ClipboardUrlModel() :
    StringModel(),
    m_enabled(false)
{
}

ClipboardUrlModel::~ClipboardUrlModel() {
    self = 0;
}

ClipboardUrlModel* ClipboardUrlModel::instance() {
    return self ? self : self = new ClipboardUrlModel;
}

bool ClipboardUrlModel::isEnabled() const {
    return m_enabled;
}

void ClipboardUrlModel::setEnabled(bool enabled) {
    if (enabled != isEnabled()) {
        m_enabled = enabled;
        emit enabledChanged(enabled);

        if (enabled) {
            onClipboardDataChanged();
            connect(QApplication::clipboard(), SIGNAL(dataChanged()), this, SLOT(onClipboardDataChanged()));
        }
        else {
            disconnect(QApplication::clipboard(), SIGNAL(dataChanged()), this, SLOT(onClipboardDataChanged()));
        }
    }
}

void ClipboardUrlModel::restore() {
    QFile file(APP_CONFIG_PATH + "clipboardurls");

    if (!file.open(QFile::ReadOnly | QFile::Text)) {
        return;
    }
    
    QTextStream stream(&file);

    while (!stream.atEnd()) {
        append(stream.readLine());
    }

    file.close();
}

void ClipboardUrlModel::save() {
    QFile file(APP_CONFIG_PATH + "clipboardurls");

    if (!file.open(QFile::WriteOnly | QFile::Text)) {
        return;
    }

    QTextStream stream(&file);

    for (int i = 0; i < m_items.size(); i++) {
        stream << m_items.at(i) << "\n";
    }

    file.close();
}

void ClipboardUrlModel::onClipboardDataChanged() {
    QStringList urls = QApplication::clipboard()->text().split(QRegExp("\\s+"), QString::SkipEmptyParts);
    urls.removeDuplicates();

    foreach (const QString &url, urls) {
        if (!m_items.contains(url)) {
            if (!Reources::getResourceFromUrl(url).isEmpty()) {
                Logger::log("ClipboardUrlModel::onClipboardDataChanged(): URL added: " + url);
                append(url);
            }
        }
    }
}
