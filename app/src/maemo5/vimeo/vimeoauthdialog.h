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

#ifndef VIMEOAUTHDIALOG_H
#define VIMEOAUTHDIALOG_H

#include "dialog.h"

class WebView;
class QUrl;
class QVBoxLayout;

class VimeoAuthDialog : public Dialog
{
    Q_OBJECT
    
public:
    explicit VimeoAuthDialog(QWidget *parent = 0);
    
protected:
    void showEvent(QShowEvent *e);
    
private Q_SLOTS:
    void onWebViewUrlChanged(const QUrl &url);
    
Q_SIGNALS:
    void codeReady(const QString &code);
    
private:
    WebView *m_view;
    QVBoxLayout *m_layout;
};

#endif // VIMEOAUTHDIALOG_H
