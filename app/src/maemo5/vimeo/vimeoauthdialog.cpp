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

#include "vimeoauthdialog.h"
#include "logger.h"
#include "vimeo.h"
#include "webview.h"
#include <QVBoxLayout>

VimeoAuthDialog::VimeoAuthDialog(QWidget *parent) :
    Dialog(parent),
    m_view(new WebView(this)),
    m_layout(new QVBoxLayout(this))
{
    setWindowTitle(tr("Authentication"));
    setMinimumHeight(360);
    
    m_layout->addWidget(m_view);
    
    connect(m_view, SIGNAL(urlChanged(QUrl)), this, SLOT(onWebViewUrlChanged(QUrl)));
    connect(m_view, SIGNAL(loadStarted()), this, SLOT(showProgressIndicator()));
    connect(m_view, SIGNAL(loadFinished(bool)), this, SLOT(hideProgressIndicator()));
}

QString VimeoAuthDialog::code() const {
    return m_code;
}

void VimeoAuthDialog::setCode(const QString &code) {
    m_code = code;
}

void VimeoAuthDialog::login() {
    setCode(QString());
    m_view->setUrl(Vimeo::authUrl());
}

void VimeoAuthDialog::onWebViewUrlChanged(const QUrl &url) {
    Logger::log("VimeoAuthDialog::onWebViewUrlChanged(): " + url.toString());
    
    if (url.toString() == "https://vimeo.com/") {
        // Work-around as for some reason Vimeo sends you to the homepage on the first attempt.
        m_view->setUrl(Vimeo::authUrl());
    }
    else if (url.hasQueryItem("code")) {
        setCode(url.queryItemValue("code"));
        accept();
    }
}
