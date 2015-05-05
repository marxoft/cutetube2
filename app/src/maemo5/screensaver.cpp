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

#include "screensaver.h"
#include <X11/Xlib.h>
#include <X11/Xatom.h>
#include <QX11Info>

void preventScreenBlanking(WId windowId, bool enabled) {
    Atom atom = XInternAtom(QX11Info::display() , "_HILDON_DO_NOT_DISTURB", False);

    if (enabled) {
        long state = 1;
        XChangeProperty(
                    QX11Info::display(),
                    windowId,
                    atom,
                    XA_INTEGER,
                    32,
                    PropModeReplace,
                    (unsigned char *) &state,
                    1);
    }
    else {
        XDeleteProperty(QX11Info::display(), windowId, atom);
    }
}
