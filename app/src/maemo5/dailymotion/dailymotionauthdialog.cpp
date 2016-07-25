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

#include "dailymotionauthdialog.h"
#include "dailymotion.h"
#include "logger.h"
#include "webview.h"
#include <QVBoxLayout>

DailymotionAuthDialog::DailymotionAuthDialog(QWidget *parent) :
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

QString DailymotionAuthDialog::code() const {
    return m_code;
}

void DailymotionAuthDialog::setCode(const QString &code) {
    m_code = code;
}

void DailymotionAuthDialog::login() {
    setCode(QString());
    m_view->setUrl(Dailymotion::authUrl());
}

void DailymotionAuthDialog::onWebViewUrlChanged(const QUrl &url) {
    Logger::log("DailymotionAuthDialog::onWebViewUrlChanged(): " + url.toString());
    
    if (url.hasQueryItem("code")) {
        setCode(url.queryItemValue("code"));
        accept();
    }
    else if ((url.hasQueryItem("reason")) && (url.queryItemValue("reason") == "access_denied")) {
        reject();
    }
}
