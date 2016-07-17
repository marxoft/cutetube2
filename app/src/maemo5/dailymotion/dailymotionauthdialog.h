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

#ifndef DAILYMOTIONAUTHDIALOG_H
#define DAILYMOTIONAUTHDIALOG_H

#include "dialog.h"

class WebView;
class QUrl;
class QVBoxLayout;

class DailymotionAuthDialog : public Dialog
{
    Q_OBJECT

    Q_PROPERTY(QString code READ code)
    
public:
    explicit DailymotionAuthDialog(QWidget *parent = 0);

    QString code() const;
    
public Q_SLOTS:
    void login();
    
private Q_SLOTS:
    void onWebViewUrlChanged(const QUrl &url);
    
private:
    void setCode(const QString &code);
    
    WebView *m_view;
    QVBoxLayout *m_layout;

    QString m_code;
};

#endif // DAILYMOTIONAUTHDIALOG_H
