 /****************************************************************************
 **
 ** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
 ** All rights reserved.
 ** Contact: Nokia Corporation (qt-info@nokia.com)
 **
 ** This file is part of the examples of the Qt Toolkit.
 **
 ** $QT_BEGIN_LICENSE:BSD$
 ** You may use this file under the terms of the BSD license as follows:
 **
 ** "Redistribution and use in source and binary forms, with or without
 ** modification, are permitted provided that the following conditions are
 ** met:
 **   * Redistributions of source code must retain the above copyright
 **     notice, this list of conditions and the following disclaimer.
 **   * Redistributions in binary form must reproduce the above copyright
 **     notice, this list of conditions and the following disclaimer in
 **     the documentation and/or other materials provided with the
 **     distribution.
 **   * Neither the name of Nokia Corporation and its Subsidiary(-ies) nor
 **     the names of its contributors may be used to endorse or promote
 **     products derived from this software without specific prior written
 **     permission.
 **
 ** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 ** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 ** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 ** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 ** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 ** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 ** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 ** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 ** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 ** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 ** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
 ** $QT_END_LICENSE$
 **
 ****************************************************************************/

 #ifndef QWEBVIEWSELECTIONSUPPRESSOR_H
 #define QWEBVIEWSELECTIONSUPPRESSOR_H

 #include <QtWebKit/qwebview.h>
 #include <QtGui/qevent.h>

 class QWebViewSelectionSuppressor : public QObject
 {
     Q_OBJECT
 public:
     QWebViewSelectionSuppressor(QWebView *v)
         : QObject(v), view(v), enabled(false), mousePressed(false)
     {
         Q_ASSERT(view);
         enable();
     }

     inline void enable()
     {
         if (enabled)
             return;
         view->installEventFilter(this);
         enabled = true;
     }

     inline void disable()
     {
         if (!enabled)
             return;
         view->removeEventFilter(this);
         enabled = false;
     }

     inline bool isEnabled() const
     {
         return enabled;
     }

 protected:
     inline bool eventFilter(QObject *, QEvent *e);

 private:
     QWebView *view;
     bool enabled;
     bool mousePressed;
 };

 bool QWebViewSelectionSuppressor::eventFilter(QObject *, QEvent *e)
 {
     switch (e->type()) {
     case QEvent::MouseButtonPress:
         if (static_cast<QMouseEvent *>(e)->button() == Qt::LeftButton)
             mousePressed = true;
         break;
     case QEvent::MouseButtonRelease:
         if (static_cast<QMouseEvent *>(e)->button() == Qt::LeftButton)
             mousePressed = false;
         break;
     case QEvent::MouseMove:
         if (mousePressed)
             return true;
         break;
     default:
         break;
     }
     return false;
 }

 #endif
