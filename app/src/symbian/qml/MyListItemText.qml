/*
 * Copyright (C) 2015 Stuart Howarth <showarth@marxoft.co.uk>
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms and conditions of the GNU Lesser General Public License,
 * version 3, as published by the Free Software Foundation.
 *
 * This program is distributed in the hope it will be useful, but WITHOUT ANY
 * WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for
 * more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program; if not, write to the Free Software Foundation,
 * Inc., 51 Franklin St - Fifth Floor, Boston, MA 02110-1301 USA.
 */

import QtQuick 1.1

Text {
    id: root
    property string mode: "normal"
    property string role: "Title"
    property bool platformInverted: false

    // Also role "Heading" taken into account although not explicitely used in evaluations below
    font {
        family: platformStyle.fontFamilyRegular
        pixelSize: (role == "Title" || role == "SelectionTitle") ? platformStyle.fontSizeLarge : platformStyle.fontSizeSmall
        weight: (role == "SubTitle" || role == "SelectionSubTitle") ? Font.Light : Font.Normal
    }
    color: internal.normalColor
    elide: Text.ElideRight
    horizontalAlignment: root.role != "Heading" ? Text.AlignLeft : Text.AlignRight
    clip: true

    // Performance optimization:
    // Use value assignment when property changes instead of binding to js function
    onModeChanged: color = internal.getColor()
    onPlatformInvertedChanged: {
        internal.colorMid = platformInverted ? platformStyle.colorNormalMidInverted
                                                  : platformStyle.colorNormalMid;
        internal.colorLight = platformInverted ? platformStyle.colorNormalLightInverted
                                                    : platformStyle.colorNormalLight;
        color = internal.getColor();
    }

    QtObject {
        id: internal

        // Performance optmization:
        // Use tertiary operations even though it doesn't look that good
        property color colorMid: root.platformInverted ? platformStyle.colorNormalMidInverted
                                                       : platformStyle.colorNormalMid
        property color colorLight: root.platformInverted ? platformStyle.colorNormalLightInverted
                                                         : platformStyle.colorNormalLight
        property color normalColor: (root.role == "SelectionSubTitle" || root.role == "SubTitle")
                                    ? colorMid : colorLight

        function getColor() {
            if (root.mode == "pressed")
                return root.platformInverted ? platformStyle.colorPressedInverted
                                             : platformStyle.colorPressed
            else if (root.mode == "highlighted")
                return root.platformInverted ? platformStyle.colorHighlightedInverted
                                             : platformStyle.colorHighlighted
            else if (root.mode == "disabled")
                return root.platformInverted ? platformStyle.colorDisabledLightInverted
                                             : platformStyle.colorDisabledLight
            else
                return normalColor
        }
    }
}
