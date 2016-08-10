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

#include "textbrowser.h"

TextBrowser::TextBrowser(QWidget *parent) :
    QTextBrowser(parent)
{
    setOpenLinks(false);
    setTextInteractionFlags(Qt::LinksAccessibleByMouse);
    setFrameStyle(QTextEdit::NoFrame);
    setStyleSheet(QString("color: %1; background: transparent")
                         .arg(palette().color(QPalette::Text).name()));
    
    //connect(this, SIGNAL(textChanged()), this, SLOT(onTextChanged()));
}

void TextBrowser::showEvent(QShowEvent *e) {
    QTextBrowser::showEvent(e);
    setFixedHeight(document()->size().height());
}

void TextBrowser::onTextChanged() {
    if (isVisible()) {
        setFixedHeight(document()->size().height());
    }
}
