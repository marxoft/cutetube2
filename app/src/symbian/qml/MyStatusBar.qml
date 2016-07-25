/****************************************************************************
**
** Copyright (C) 2011 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the Qt Components project.
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

import QtQuick 1.1
import com.nokia.symbian 1.1

Item {
    id: root

    implicitWidth: screen.width
    implicitHeight: privateStyle.statusBarHeight
    property bool platformInverted: false
    property alias title: titleText.text

    QtObject {
        id: priv
        objectName: "priv"

        property bool clickedOpensStatusPanel: symbian.s60Version == Symbian.SV_S60_5_2 ? true : false
        property int contentHeight: Math.round(privateStyle.statusBarHeight * 18 / 26)
        property int paddingSmallOneQuarter: Math.round(platformStyle.paddingSmall / 4)
        property int paddingSmallThreeQuarters: Math.round(platformStyle.paddingSmall * 3 / 4)

        function signalWidthPercentage(signalStrength) {
            if (signalStrength < 10)
                return 0;
            else if (signalStrength < 20)
                return 1/5;
            else if (signalStrength < 30)
                return 2/5;
            else if (signalStrength < 60)
                return 3/5;
            else if (signalStrength < 100)
                return 4/5;
            else
                return 1;
        }
    }

    MouseArea {
        id: mouseArea
        anchors.fill: parent

        onClicked: {
            if (priv.clickedOpensStatusPanel) {
                privateStyle.play(Symbian.PopUp)
                platformPopupManager.privateShowIndicatorPopup()
            }
        }
        onPressed: {
            if (!priv.clickedOpensStatusPanel) {
                privateStyle.play(Symbian.PopUp)
                platformPopupManager.privateShowIndicatorPopup()
                // reset MouseArea state since status panel window eats the release event
                symbian.privateSendMouseRelease(mouseArea)
            }
        }
    }

    BorderImage {
        source: privateStyle.imagePath("qtg_fr_statusbar", root.platformInverted)
        anchors.fill: parent
        width: parent.width
        
        // title text
        Text {
            id: titleText
            color: root.platformInverted ? platformStyle.colorNormalDarkInverted
                                         : platformStyle.colorNormalLight
            anchors.verticalCenter: parent.verticalCenter
            anchors.left: parent.left
            anchors.leftMargin: platformStyle.paddingSmall
            anchors.right: progressIndicator.visible ? progressIndicator.left : indicators.left
            anchors.rightMargin: platformStyle.paddingSmall
            elide: Text.ElideRight
            font {
                family: platformStyle.fontFamilyRegular;
                pixelSize: priv.contentHeight
                weight: Font.Light
            }
        }

        BusyIndicator {
            id: progressIndicator

            width: priv.contentHeight
            height: priv.contentHeight
            anchors {
                right: indicators.left
                rightMargin: platformStyle.paddingSmall
                verticalCenter: parent.verticalCenter
            }
            running: visible
            visible: (stack.currentPage) && (stack.currentPage.showProgressIndicator)
        }

        UniversalIndicators {
            id: indicators
            height: indicatorHeight
            anchors {
                right: networkMode.left
                rightMargin: priv.paddingSmallOneQuarter
                verticalCenter: parent.verticalCenter
                }

            width: priv.paddingSmallOneQuarter * 2 + 3 * indicatorWidth

            indicatorColor: root.platformInverted ? platformStyle.colorNormalDarkInverted
                                                  : platformStyle.colorNormalLight
            indicatorWidth: priv.contentHeight // same as height
            indicatorHeight: priv.contentHeight
            indicatorPadding: priv.paddingSmallOneQuarter
            maxIndicatorCount: 3
        }

        // icon for network signal type e.g. 3G, GPRS etc
        NetworkIndicator {
            id: networkMode
            height: priv.contentHeight
            width: priv.contentHeight // same as height
            anchors.verticalCenter: parent.verticalCenter
            anchors.right: offline ? batteryBackground.left : signalBackground.left
            anchors.rightMargin: priv.paddingSmallThreeQuarters
            color: root.platformInverted ? platformStyle.colorNormalDarkInverted
                                         : platformStyle.colorNormalLight
        }
        // signal strength
        Image {
            id: signalBackground
            visible: !networkMode.offline
            sourceSize.height: priv.contentHeight
            sourceSize.width: Math.round(privateStyle.statusBarHeight * 19 / 26)
            anchors.verticalCenter: parent.verticalCenter
            anchors.right: batteryBackground.left
            anchors.rightMargin: priv.paddingSmallThreeQuarters
            fillMode: Image.PreserveAspectFit
            source: privateStyle.imagePath("qtg_graf_signal_level_bg", root.platformInverted)
            Item {
                id: signalLevelItem
                anchors.left: parent.left
                anchors.top: parent.top
                height: parent.height
                width: priv.signalWidthPercentage(privateNetworkInfo.networkSignalStrength) * parent.width
                clip: true
                LayoutMirroring.enabled: false

                Image {
                    sourceSize.width: signalBackground.sourceSize.width
                    sourceSize.height: signalBackground.sourceSize.height
                    fillMode: Image.PreserveAspectFit
                    source: privateStyle.imagePath("qtg_graf_signal_level_full", root.platformInverted)
                }
            }
        }
        // battery indicator
        Image {
            id: batteryBackground
            anchors.verticalCenter: parent.verticalCenter
            anchors.right: timeItem.left
            anchors.rightMargin: priv.paddingSmallThreeQuarters
            sourceSize.height: priv.contentHeight
            sourceSize.width: Math.round(privateStyle.statusBarHeight * 24 / 26)
            fillMode: Image.PreserveAspectFit
            source: privateStyle.imagePath((privateBatteryInfo.powerSaveModeEnabled ?
                "qtg_graf_battery_level_psm_bg" :
                "qtg_graf_battery_level_bg"), root.platformInverted)

            Item {
                id: batteryLevel

                property int animatedLevel

                anchors.left: parent.left
                anchors.top: parent.top
                width: Math.round(privateStyle.statusBarHeight
                    * ((privateBatteryInfo.charging ? Math.floor(animatedLevel / 100) :
                        privateBatteryInfo.batteryLevel) + 2) / 13)
                height: parent.height
                clip: true
                LayoutMirroring.enabled: false

                Image {
                    sourceSize.width: batteryBackground.sourceSize.width
                    sourceSize.height: batteryBackground.sourceSize.height

                    fillMode: Image.PreserveAspectFit

                    // Battery state mappings: Levels 0 and 1 are low, 2-4 are medium, 5-7 are full.
                    // Currently all levels use same graphics with white color.

                    source: privateStyle.imagePath((privateBatteryInfo.powerSaveModeEnabled ?
                        "qtg_graf_battery_level_psm_full" :
                        "qtg_graf_battery_level_full"), root.platformInverted)
                }
            }

            Image {
                // power save mode indicator
                anchors.fill: parent
                sourceSize.width: parent.sourceSize.width
                sourceSize.height: parent.sourceSize.height
                source: privateStyle.imagePath("qtg_graf_battery_psm")
                visible: privateBatteryInfo.powerSaveModeEnabled
            }

            NumberAnimation {
                id: batteryChargingAnimation
                loops: Animation.Infinite
                running: privateBatteryInfo.charging
                target: batteryLevel
                property: "animatedLevel"
                // Use bigger range (compared to 0-7) in order to make the animation smoother.
                from: 0
                to: 799
                duration: 3500
            }
        }
        // clock
        Text {
            id: timeItem
            width: Math.round(privateStyle.statusBarHeight * 44 / 26)
            color: root.platformInverted ? platformStyle.colorNormalDarkInverted
                                         : platformStyle.colorNormalLight
            anchors.verticalCenter: parent.verticalCenter
            anchors.right: parent.right
            anchors.rightMargin: platformStyle.paddingSmall
            horizontalAlignment: Text.AlignRight
            text: symbian.currentTime
            font {
                family: platformStyle.fontFamilyRegular;
                pixelSize: priv.contentHeight
                weight: Font.Light
            }
        }
    }
}

