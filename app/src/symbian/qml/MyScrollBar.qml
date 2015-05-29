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
import com.nokia.symbian 1.1
import "scripts/SectionScroller.js" as Sections

Item {
    id: root

    property Flickable flickableItem: null
    property int orientation: Qt.Vertical
    property bool interactive: true
    property int policy: Symbian.ScrollBarWhenScrolling
    property bool privateSectionScroller: false
    property bool platformInverted: false

    //implicit values for qml designer when no Flickable is present
    implicitHeight: privateStyle.scrollBarThickness * (orientation == Qt.Vertical ? 3 : 1)
    implicitWidth: privateStyle.scrollBarThickness * (orientation == Qt.Horizontal ? 3 : 1)
    height: {
        if (flickableItem && orientation == Qt.Vertical)
            return Math.floor(Math.round(flickableItem.height) - anchors.topMargin - anchors.bottomMargin)
        return undefined
    }
    width: {
        if (flickableItem && orientation == Qt.Horizontal)
            return Math.floor(Math.round(flickableItem.width) - anchors.leftMargin - anchors.rightMargin)
        return undefined
    }
    opacity: internal.rootOpacity
    parent: flickableItem
    anchors {
        top: parent ? parent.top : undefined
        right: parent ? parent.right : undefined
    }

    onFlickableItemChanged: {
        if (flickableItem.hasOwnProperty("model")) {
            internal.initDirtyObserver();
        }

        if (flickableItem.hasOwnProperty("scrollIndicator")) {
            flickableItem.scrollIndicator = root;
        }
    }

    onPrivateSectionScrollerChanged: internal.initSectionScroller();

    //For showing explicitly a ScrollBar if policy is Symbian.ScrollBarWhenScrolling
    function flash(type) {
        if (policy == Symbian.ScrollBarWhenScrolling && internal.scrollBarNeeded) {
            flashEffect.type = (type == undefined) ? Symbian.FadeOut : type
            flashEffect.restart()
        }
    }

    Connections {
        target: screen
        onCurrentOrientationChanged: flash()
    }

    Connections {
        target: root.privateSectionScroller ? flickableItem : null
        onModelChanged: internal.initSectionScroller()
    }

    Loader {
        id: listItemInteraction
        sourceComponent: flickableItem && flickableItem.hasOwnProperty("currentIndex") && flickableItem.activeFocus ? listItemInteractionComponent : undefined
    }

    Component {
        id: listItemInteractionComponent
        Item {
            Connections {
                target: symbian
                onListInteractionModeChanged: flash()
                onPrivateListItemKeyNavigation: flash()
            }
        }
    }

    Connections {
        target: !flickableItem.model ? null : flickableItem
        onModelChanged: {
            if (flickableItem && flickableItem.model)
                internal.initDirtyObserver();
        }
    }

    Timer {
        id: dirtyTimer
        interval: 100
        running: false
        onTriggered: {
            Sections.initSectionData(flickableItem);
            internal.modelDirty = false;
        }
    }

    QtObject {
        id: internal
        property bool modelDirty: false
        property int hideTimeout: 500
        property int pageStepY: flickableItem ? Math.floor(flickableItem.visibleArea.heightRatio * flickableItem.contentHeight) : NaN
        property int pageStepX: flickableItem ? Math.floor(flickableItem.visibleArea.widthRatio * flickableItem.contentWidth) : NaN
        property int handleY: flickableItem ? Math.floor(handle.y / flickableItem.height * flickableItem.contentHeight) : NaN
        property int handleX: flickableItem ? Math.floor(handle.x / flickableItem.width * flickableItem.contentWidth) : NaN
        property int maximumY: flickableItem ? Math.floor(Math.min(flickableItem.contentHeight - root.height, flickableItem.contentY)) : NaN
        property int maximumX: flickableItem ? Math.floor(Math.min(flickableItem.contentWidth - root.width, flickableItem.contentX)) : NaN
        property bool scrollBarNeeded: root.visible && hasScrollableContent()
        //Sets currentSection to empty string when flickableItem.currentSection is null
        property string currentSection: flickableItem ? flickableItem.currentSection || "" : ""
        //To be able to pressed on trackMouseArea, opacity needs to be greater than 0
        property real rootOpacity: root.privateSectionScroller ? 0.01 : 0

        // The following properties are used for calculating handle position and size:
        // Minimum allowed handle length
        property real minHandleLength: 3 * Math.floor(privateStyle.scrollBarThickness)
        // Scroll bar "container" length
        property real totalLength: !flickableItem ? NaN : (orientation == Qt.Vertical ? root.height : root.width)
        property real relativeHandlePosition: !flickableItem ? NaN : (orientation == Qt.Vertical
                                                                      ? flickableItem.visibleArea.yPosition
                                                                      : flickableItem.visibleArea.xPosition)
        property real lengthRatio: !flickableItem ? NaN : (orientation == Qt.Vertical
                                                           ? flickableItem.visibleArea.heightRatio
                                                           : flickableItem.visibleArea.widthRatio)
        // Scroll bar handle length under normal circumstances
        property real staticHandleLength: Math.max(minHandleLength, lengthRatio * totalLength)
        // Adjustment factor needed to calculate correct position, which is needed because
        // relativeHandlePosition and lengthRatio are assuming default handle length. Dividend is
        // the maximum handle position taking into account restricted minimum handle length, whereas
        // divisor is the maximum handle position if default handle size would be used.
        property real handlePositionAdjustment: (totalLength - staticHandleLength) / (totalLength * (1 - lengthRatio))
        property real handlePosition
        handlePosition: {
            // handle's position, which is adjusted to take into account non-default handle
            // length and restricted never to exceed flickable boundaries
            var pos = totalLength * relativeHandlePosition * handlePositionAdjustment
            return Math.min(Math.max(0, pos), totalLength - minHandleLength)
        }
        property real dynamicHandleLength
        dynamicHandleLength: {
            // dynamic handle length, which may differ from static due to bounds behavior, i.e.
            // handle gets shorter when content exceeds flickable boundaries
            var length = 0
            if (relativeHandlePosition >= 1 - lengthRatio) // overflow
                length = 1 - relativeHandlePosition
            else if (relativeHandlePosition < 0) // underflow
                length = lengthRatio + relativeHandlePosition
            else
                length = lengthRatio
            return Math.max(minHandleLength, length * totalLength)
        }

        // Show flash effect in case if no flicking (flickableItem.moving) has occured
        // but there is a need to indicate scrollable content.
        onScrollBarNeededChanged: {
            if (stateGroup.state == "") {
                if (scrollBarNeeded)
                    flash()
                else
                    idleEffect.restart()
            }
        }

        function initDirtyObserver() {
            Sections.initSectionData(flickableItem);
            function dirtyObserver() {
                if (!internal.modelDirty) {
                    internal.modelDirty = true;
                    dirtyTimer.running = true;
                }
            }

            if (flickableItem.model.countChanged)
                flickableItem.model.countChanged.connect(dirtyObserver);

            if (flickableItem.model.itemsChanged)
                flickableItem.model.itemsChanged.connect(dirtyObserver);

            if (flickableItem.model.itemsInserted)
                flickableItem.model.itemsInserted.connect(dirtyObserver);

            if (flickableItem.model.itemsMoved)
                flickableItem.model.itemsMoved.connect(dirtyObserver);

            if (flickableItem.model.itemsRemoved)
                flickableItem.model.itemsRemoved.connect(dirtyObserver);
        }

        /**
         * Checks whether ScrollBar is needed or not
         * based on Flickable visibleArea height and width ratios
         */
        function hasScrollableContent() {
            if (!flickableItem)
                return false
            var ratio = orientation == Qt.Vertical ? flickableItem.visibleArea.heightRatio : flickableItem.visibleArea.widthRatio
            return ratio < 1.0 && ratio > 0
        }
        /**
         * Does Page by Page movement of flickableItem
         * when ScrollBar Track is being clicked/pressed
         *
         * @see #moveToLongTapPosition
         */
        function doPageStep() {
            if (orientation == Qt.Vertical) {
                if (trackMouseArea.mouseY > (handle.height / 2 + handle.y)) {
                    flickableItem.contentY += pageStepY
                    flickableItem.contentY = maximumY
                }
                else if (trackMouseArea.mouseY < (handle.height / 2 + handle.y)) {
                    flickableItem.contentY -= pageStepY
                    flickableItem.contentY = Math.max(0, flickableItem.contentY)
                }
            } else {
                if (trackMouseArea.mouseX > (handle.width / 2 + handle.x)) {
                    flickableItem.contentX += pageStepX
                    flickableItem.contentX = maximumX
                }
                else if (trackMouseArea.mouseX < (handle.width / 2 + handle.x)) {
                    flickableItem.contentX -= pageStepX
                    flickableItem.contentX = Math.max(0, flickableItem.contentX)
                }
            }
        }
        /**
         * Does movement of flickableItem
         * when ScrollBar Handle is being dragged
         */
        function moveToHandlePosition() {
            if (orientation == Qt.Vertical)
                flickableItem.contentY = handleY
            else
                flickableItem.contentX = handleX
        }
        /**
         * Moves flickableItem's content according to given mouseArea movement
         * when mouseArea is pressed long
         * Tries to position the handle and content in center of mouse position enough
         *
         * @see #doPageStep
         */
        function moveToLongTapPosition(mouseArea) {
            if (orientation == Qt.Vertical) {
                if (Math.abs(mouseArea.mouseY - (handle.height / 2 + handle.y)) < privateStyle.scrollBarThickness)
                    return //if change is not remarkable enough, do nothing otherwise it would cause annoying flickering effect
                if (mouseArea.mouseY > (handle.height / 2 + handle.y)) {
                    flickableItem.contentY += Math.floor(privateStyle.scrollBarThickness)
                    flickableItem.contentY = maximumY
                }
                else if (mouseArea.mouseY < (handle.height / 2 + handle.y)) {
                    flickableItem.contentY -= Math.floor(privateStyle.scrollBarThickness)
                    flickableItem.contentY = Math.floor(Math.max(0, flickableItem.contentY))
                }
            } else {
                if (Math.abs(mouseArea.mouseX - (handle.width / 2 + handle.x)) < privateStyle.scrollBarThickness)
                    return //if change is not remarkable enough, do nothing otherwise it would cause annoying flickering effect
                if (mouseArea.mouseX > (handle.width / 2 + handle.x)) {
                    flickableItem.contentX += Math.floor(privateStyle.scrollBarThickness)
                    flickableItem.contentX = maximumX
                }
                else if (mouseArea.mouseX < (handle.width / 2 + handle.x)) {
                    flickableItem.contentX -= Math.floor(privateStyle.scrollBarThickness)
                    flickableItem.contentX = Math.floor(Math.max(0, flickableItem.contentX))
                }
            }
        }

        function adjustContentPosition(y) {
            if (y < 0 || y > trackMouseArea.height)
                return;

            var sect = Sections.closestSection(y / trackMouseArea.height);
            var idx = Sections.indexOf(sect);
            currentSection = sect;
            flickableItem.positionViewAtIndex(idx, ListView.Beginning);
        }

        function initSectionScroller() {
            if (root.privateSectionScroller && flickableItem && flickableItem.model)
                Sections.initSectionData(flickableItem);
        }
    }

    BorderImage {
        id: track
        objectName: "track"
        source: privateStyle.imagePath((orientation == Qt.Vertical
                                        ? "qtg_fr_scrollbar_v_track_normal"
                                        : "qtg_fr_scrollbar_h_track_normal"),
                                       root.platformInverted)
        visible: interactive
        anchors.fill: parent
        border.right: orientation == Qt.Horizontal ? 7 : 0
        border.left: orientation == Qt.Horizontal ? 7 : 0
        border.top: orientation == Qt.Vertical ? 7 : 0
        border.bottom: orientation == Qt.Vertical ? 7 : 0
        onVisibleChanged: { idleEffect.complete(); flashEffect.complete() }
    }

    Loader {
        id: sectionScrollBackground
        anchors.right: trackMouseArea.right
        width: flickableItem ? flickableItem.width : 0
        height: platformStyle.fontSizeMedium * 5
        sourceComponent: root.privateSectionScroller ? sectionScrollComponent : null
    }

    Component {
        id: sectionScrollComponent
        BorderImage {
            id: indexFeedbackBackground
            objectName: "indexFeedbackBackground"
            source: privateStyle.imagePath("qtg_fr_popup_transparent", root.platformInverted)
            border { left: platformStyle.borderSizeMedium; top: platformStyle.borderSizeMedium; right: platformStyle.borderSizeMedium; bottom: platformStyle.borderSizeMedium }
            visible: trackMouseArea.pressed
            anchors.fill: parent
            Text {
                id: indexFeedbackText
                objectName: "indexFeedbackText"
                color: root.platformInverted ? platformStyle.colorNormalDarkInverted // intentionally dark inverted
                                             : platformStyle.colorNormalLight
                anchors {
                    left: parent.left;
                    leftMargin: platformStyle.paddingLarge;
                    right: parent.right;
                    rightMargin: platformStyle.paddingLarge;
                    verticalCenter: parent.verticalCenter
                }
                font {
                    family: platformStyle.fontFamilyRegular;
                    pixelSize: indexFeedbackText.text.length == 1 ? platformStyle.fontSizeMedium * 4 : platformStyle.fontSizeMedium * 2;
                    capitalization: indexFeedbackText.text.length == 1 ? Font.AllUppercase : Font.MixedCase
                }
                text: internal.currentSection
                horizontalAlignment: Text.AlignLeft
                elide: Text.ElideRight
            }
            states: [
                State {
                    when: (handle.y + (handle.height / 2)) - (sectionScrollBackground.height / 2) < 0
                    AnchorChanges {
                        target: sectionScrollBackground
                        anchors { verticalCenter: undefined; top: track.top; bottom: undefined }
                    }
                },
                State {
                    when: (handle.y + (handle.height / 2)) + (sectionScrollBackground.height / 2) >= track.height
                    AnchorChanges {
                        target: sectionScrollBackground
                        anchors { verticalCenter: undefined; top: undefined; bottom: track.bottom }
                    }
                },
                State {
                    when: (handle.y + (handle.height / 2)) - (sectionScrollBackground.height / 2) >= 0
                    AnchorChanges {
                        target: sectionScrollBackground
                        anchors { verticalCenter: handle.verticalCenter; top: undefined; bottom: undefined }
                    }
                }
            ]
        }
    }

    // MouseArea for the move content "page by page" by tapping and scroll to press-and-hold position
    MouseArea {
        id: trackMouseArea
        objectName: "trackMouseArea"
        property bool longPressed: false
        enabled: root.privateSectionScroller || interactive
        anchors {
            top: root.privateSectionScroller ? parent.top : undefined;
            bottom: root.privateSectionScroller ? parent.bottom : undefined;
            right: root.privateSectionScroller ? parent.right : undefined;
            fill: root.privateSectionScroller ? undefined : (flickableItem ? track : undefined)
        }
        width: root.privateSectionScroller ? privateStyle.scrollBarThickness * 3 : undefined
        drag {
            target: root.privateSectionScroller ? sectionScrollBackground : undefined
            // axis is set XandY to prevent flickable from stealing the mouse event
            // SectionScroller is anchored to the right side of the mouse area so the user
            // won't be able to drag it along the X axis
            axis: root.privateSectionScroller ? Drag.XandYAxis : 0
            minimumY: root.privateSectionScroller ? (flickableItem ? flickableItem.y : 0) : 0
            maximumY: root.privateSectionScroller ? (flickableItem ? (trackMouseArea.height - sectionScrollBackground.height) : 0) : 0
        }

        onPressAndHold: {
            if (!root.privateSectionScroller)
                longPressed = true
        }

        onReleased: longPressed = false

        onPositionChanged: {
            if (root.privateSectionScroller)
                internal.adjustContentPosition(trackMouseArea.mouseY);
        }

        onPressedChanged: {
            if (root.privateSectionScroller && trackMouseArea.pressed)
                internal.adjustContentPosition(trackMouseArea.mouseY);
        }
    }
    Timer {
        id: pressAndHoldTimer
        running: trackMouseArea.longPressed
        interval: 50
        repeat: true
        onTriggered: { internal.moveToLongTapPosition(trackMouseArea); privateStyle.play(Symbian.SensitiveSlider) }
    }

    BorderImage {
        id: handle
        objectName: "handle"
        source: privateStyle.imagePath(handleFileName(), root.platformInverted)
        x: orientation == Qt.Horizontal ? internal.handlePosition : NaN
        y: orientation == Qt.Vertical ? internal.handlePosition : NaN
        height: orientation == Qt.Vertical ? internal.dynamicHandleLength : root.height
        width: orientation == Qt.Horizontal ? internal.dynamicHandleLength : root.width
        border.right: orientation == Qt.Horizontal ? 7 : 0
        border.left: orientation == Qt.Horizontal ? 7 : 0
        border.top: orientation == Qt.Vertical ? 7 : 0
        border.bottom: orientation == Qt.Vertical ? 7 : 0

        function handleFileName() {
            var fileName = (orientation == Qt.Vertical ? "qtg_fr_scrollbar_v_handle_" :
                                                         "qtg_fr_scrollbar_h_handle_")
            if (!interactive)
                fileName += "indicative"
            else if (handleMouseArea.pressed)
                fileName += "pressed"
            else
                fileName += "normal"
            return fileName
        }
    }

    MouseArea {
        id: handleMouseArea
        objectName: "handleMouseArea"
        property real maxDragY: flickableItem ? flickableItem.height - handle.height - root.anchors.topMargin - root.anchors.bottomMargin : NaN
        property real maxDragX: flickableItem ? flickableItem.width - handle.width - root.anchors.leftMargin - root.anchors.rightMargin : NaN
        enabled: interactive && !root.privateSectionScroller
        width: orientation == Qt.Vertical ? 3 * privateStyle.scrollBarThickness : handle.width
        height: orientation == Qt.Horizontal ? 3 * privateStyle.scrollBarThickness : handle.height
        anchors {
            verticalCenter: flickableItem ? handle.verticalCenter : undefined
            horizontalCenter: flickableItem ? handle.horizontalCenter : undefined
        }
        drag {
            target: handle
            axis: orientation == Qt.Vertical ? Drag.YAxis : Drag.XAxis
            minimumY: 0
            maximumY: maxDragY
            minimumX: 0
            maximumX: maxDragX
        }
        onPositionChanged: internal.moveToHandlePosition()
    }

    PropertyAnimation {
        id: indicateEffect

        function play() {
            flashEffect.stop()
            idleEffect.stop()
            restart()
        }

        target: root
        property: "opacity"
        to: 1
        duration: 0
    }
    SequentialAnimation {
        id: idleEffect

        function play() {
            indicateEffect.stop()
            if (internal.scrollBarNeeded && root.policy == Symbian.ScrollBarWhenScrolling)
                restart()
        }

        PauseAnimation { duration: root.interactive ? 1500 : 0 }
        PropertyAnimation {
            target: root
            property: "opacity"
            to: internal.rootOpacity
            duration: internal.hideTimeout
        }
    }
    SequentialAnimation {
        id: flashEffect
        property int type: Symbian.FadeOut

        PropertyAnimation {
            target: root
            property: "opacity"
            to: 1
            duration: (flashEffect.type == Symbian.FadeInFadeOut) ? internal.hideTimeout : 0
        }
        PropertyAnimation {
            target: root
            property: "opacity"
            to: internal.rootOpacity
            duration: internal.hideTimeout
        }
    }
    StateGroup {
        id: stateGroup
        states: [
            State {
                name: "Move"
                when: handleMouseArea.pressed
            },
            State {
                name: "Stepping"
                when: trackMouseArea.longPressed
            },
            State {
                name: "Step"
                when: trackMouseArea.pressed && !trackMouseArea.longPressed && !root.privateSectionScroller
            },
            State {
                name: "Indicate"
                when: (internal.scrollBarNeeded && flickableItem.moving) ||
                      (trackMouseArea.pressed && root.privateSectionScroller)
            },
            State {
                name: ""
            }
        ]
        transitions: [
            Transition {
                to: "Move"
                ScriptAction { script: privateStyle.play(Symbian.BasicSlider) }
                ScriptAction { script: indicateEffect.play() }
            },
            Transition {
                to: "Step"
                ScriptAction { script: internal.doPageStep() }
                ScriptAction { script: privateStyle.play(Symbian.BasicSlider) }
                ScriptAction { script: indicateEffect.play() }
            },
            Transition {
                from: "Step"
                to: "Stepping"
                ScriptAction { script: indicateEffect.play() }
            },
            Transition {
                from: "Move"
                to: "Indicate"
                ScriptAction { script: privateStyle.play(Symbian.BasicSlider) }
                ScriptAction { script: indicateEffect.play() }
            },
            Transition {
                to: "Indicate"
                ScriptAction { script: indicateEffect.play() }
            },
            Transition {
                from: "Move"
                to: ""
                ScriptAction { script: privateStyle.play(Symbian.BasicSlider) }
                ScriptAction { script: idleEffect.play() }
            },
            Transition {
                to: ""
                ScriptAction { script: idleEffect.play() }
            }
        ]
    }
}
