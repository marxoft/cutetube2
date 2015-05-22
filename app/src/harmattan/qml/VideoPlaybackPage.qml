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

import QtQuick 1.1
import com.nokia.meego 1.0
import QtMultimediaKit 1.1 as Multimedia
import cuteTube 2.0
import "file:///usr/lib/qt4/imports/com/nokia/meego/UIConstants.js" as UI

MyPage {
    id: root

    property bool applicationActive: Qt.application.active

    function addVideos(videos) {
        for (var i = 0; i < videos.length; i++) {
            playbackQueue.append(videos[i]);
        }

        playbackQueue.position = 0;
    }

    function startPlayback() {
        var video = playbackQueue.get(playbackQueue.position);

        if (video.streamUrl.toString()) {
            videoPlayer.stop();
            videoPlayer.source = "";
            videoPlayer.playUrl(video.streamUrl);
        }
        else if (video.service == Resources.YOUTUBE) {
            youtubeModel.list(video.id);
        }
        else if (video.service == Resources.DAILYMOTION) {
            dailymotionModel.list(video.id);
        }
        else if (video.service == Resources.VIMEO) {
            vimeoModel.list(video.id);
        }
        else {
            pluginModel.service = video.service;
            pluginModel.list(video.id);
        }
    }

    function previous() {
        if (playbackQueue.position > 0) {
            playbackQueue.position--;
            getNextVideo();
        }
    }

    function next() {
        if (playbackQueue.position < playbackQueue.count - 1) {
            playbackQueue.position++;
            getNextVideo();
        }
        else {
            stopPlayback();
        }
    }

    function getNextVideo() {
        if (playbackQueue.position < playbackQueue.count) {
            startPlayback();
        }
    }

    function stopPlayback() {
        videoPlayer.stop();
    }

    objectName: "playbackPage"
    orientationLock: appWindow.pageStack.currentPage == root ? PageOrientation.Automatic : Settings.screenOrientation
    tools: ToolBarLayout {

        BackToolIcon {
            onClicked: {
                stopPlayback();
                appWindow.showStatusBar = true;
                appWindow.showToolBar = true;
                appStyle.cornersVisible = true;
            }
        }
    }

    VideoModel {
        id: playbackQueue

        property int position: -1
    }

    YouTubeStreamModel {
        id: youtubeModel

        onStatusChanged: {
            switch (status) {
            case 3:
                if (count > 0) {
                    var i = match("name", Settings.defaultPlaybackFormat(Resources.YOUTUBE));

                    if ((dialogLoader.sourceComponent == streamDialog) && (dialogLoader.item.status == DialogStatus.Open)) {
                        dialogLoader.item.selectedIndex = Math.max(0, i);
                    }
                    else if (i >= 0) {
                        var v = data(i, "value");
                        videoPlayer.playUrl(v.url);
                        formatLabel.text = v.height + "p";
                    }
                    else {
                        dialogLoader.sourceComponent = streamDialog;
                        dialogLoader.item.model = youtubeModel;
                        dialogLoader.item.open();
                    }
                }
                else {
                    infoBanner.showMessage(qsTr("No streams found"));
                }

                break;
            case 4:
                infoBanner.showMessage(errorString);
                break;
            default:
                break;
            }
        }
    }

    DailymotionStreamModel {
        id: dailymotionModel

        onStatusChanged: {
            switch (status) {
            case 3:
                if (count > 0) {
                    var i = match("name", Settings.defaultPlaybackFormat(Resources.DAILYMOTION));

                    if ((dialogLoader.sourceComponent == streamDialog) && (dialogLoader.item.status == DialogStatus.Open)) {
                        dialogLoader.item.selectedIndex = Math.max(0, i);
                    }
                    else if (i >= 0) {
                        var v = data(i, "value");
                        videoPlayer.playUrl(v.url);
                        formatLabel.text = v.height + "p";
                    }
                    else {
                        dialogLoader.sourceComponent = streamDialog;
                        dialogLoader.item.model = dailymotionModel;
                        dialogLoader.item.open();
                    }
                }
                else {
                    infoBanner.showMessage(qsTr("No streams found"));
                }

                break;
            case 4:
                infoBanner.showMessage(errorString);
                break;
            default:
                break;
            }
        }
    }

    VimeoStreamModel {
        id: vimeoModel

        onStatusChanged: {
            switch (status) {
            case 3:
                if (count > 0) {
                    var i = match("name", Settings.defaultPlaybackFormat(Resources.VIMEO));

                    if ((dialogLoader.sourceComponent == streamDialog) && (dialogLoader.item.status == DialogStatus.Open)) {
                        dialogLoader.item.selectedIndex = Math.max(0, i);
                    }
                    else if (i >= 0) {
                        var v = data(i, "value");
                        videoPlayer.playUrl(v.url);
                        formatLabel.text = v.height + "p";
                    }
                    else {
                        dialogLoader.sourceComponent = streamDialog;
                        dialogLoader.item.model = vimeoModel;
                        dialogLoader.item.open();
                    }
                }
                else {
                    infoBanner.showMessage(qsTr("No streams found"));
                }

                break;
            case 4:
                infoBanner.showMessage(errorString);
                break;
            default:
                break;
            }
        }
    }

    PluginStreamModel {
        id: pluginModel

        onStatusChanged: {
            switch (status) {
            case 3:
                if (count > 0) {
                    var i = match("name", Settings.defaultPlaybackFormat(service));

                    if ((dialogLoader.sourceComponent == streamDialog) && (dialogLoader.item.status == DialogStatus.Open)) {
                        dialogLoader.item.selectedIndex = Math.max(0, i);
                    }
                    else if (i >= 0) {
                        var v = data(i, "value");
                        videoPlayer.playUrl(v.url);
                        formatLabel.text = v.height + "p";
                    }
                    else {
                        dialogLoader.sourceComponent = streamDialog;
                        dialogLoader.item.model = pluginModel;
                        dialogLoader.item.open();
                    }
                }
                else {
                    infoBanner.showMessage(qsTr("No streams found"));
                }

                break;
            case 4:
                infoBanner.showMessage(errorString);
                break;
            default:
                break;
            }
        }
    }

    Binding {
        id: progressBarBinding

        target: progressBar
        property: "value"
        value: toolBar.show ? videoPlayer.position : 0
        when: platformWindow.viewMode === WindowState.Fullsize
    }

    Binding {
        id: positionLabelBinding

        target: positionLabel
        property: "text"
        value: toolBar.show ? Utils.formatMSecs(videoPlayer.position) : "00:00"
        when: platformWindow.viewMode === WindowState.Fullsize
    }

    Timer {
        interval: 10000
        repeat: true
        running: (platformWindow.viewMode === WindowState.Thumbnail) && (videoPlayer.playing)
        onTriggered: {
            progressBar.value = videoPlayer.position;
            positionLabel.text = Utils.formatMSecs(videoPlayer.position);
        }
    }

    ToolBar {
        id: toolBar

        property bool show: false

        z: 10
        anchors {
            left: videoPlayer.left
            right: videoPlayer.right
            bottom: videoPlayer.bottom
            bottomMargin: show ? 0 : -height
        }
        platformStyle: ToolBarStyle {
            id: toolBarStyle

            background: Qt.resolvedUrl("images/toolbar-background.png")
        }

        MouseArea {
            anchors.fill: parent
            z: -1
        }

        tools: ToolBarLayout {

            BackToolIcon {
                id: stopButton

                anchors.left: parent.left
                onClicked: {
                    stopPlayback();
                    appWindow.showStatusBar = true;
                    appWindow.showToolBar = true;
                    appStyle.cornersVisible = true;
                }
            }

            MyToolIcon {
                id: playButton

                anchors.left: stopButton.right
                platformIconId: "toolbar-mediacontrol-" + ((videoPlayer.paused) || (!videoPlayer.playing) ? "play" : "pause")
                onClicked: {
                    if (videoPlayer.playing) {
                        videoPlayer.paused = !videoPlayer.paused;
                        controlsTimer.restart();
                    }
                    else {
                        videoPlayer.position = 0;
                        videoPlayer.play();
                    }
                }
            }

            MyProgressBar {
                id: progressBar

                anchors {
                    left: playButton.right
                    leftMargin: UI.PADDING_DOUBLE
                    right: queueButton.left
                    rightMargin: UI.PADDING_DOUBLE
                    verticalCenter: parent.verticalCenter
                }
                indeterminate: videoPlayer.loading
                minimumValue: 0
                maximumValue: videoPlayer.duration

                Label {
                    id: positionLabel

                    anchors {
                        top: parent.bottom
                        horizontalCenter: parent.left
                    }
                    font.pixelSize: UI.FONT_SMALL
                }

                Label {
                    id: durationLabel

                    anchors {
                        top: parent.bottom
                        horizontalCenter: parent.right
                    }
                    font.pixelSize: UI.FONT_SMALL
                    text: Utils.formatMSecs(videoPlayer.duration)
                }

                SeekBubble {
                    id: seekBubble

                    anchors.bottom: parent.top
                    opacity: value != "" ? 1 : 0
                    value: (seekMouseArea.drag.active) && (seekMouseArea.posInsideDragArea)
                           ? Utils.formatMSecs(Math.floor((seekMouseArea.mouseX / seekMouseArea.width) * videoPlayer.duration)) : ""
                }

                MouseArea {
                    id: seekMouseArea

                    property bool posInsideMouseArea: false
                    property bool posInsideDragArea: (seekMouseArea.mouseX >= 0)
                                                     && (seekMouseArea.mouseX <= seekMouseArea.drag.maximumX)

                    width: parent.width
                    height: 60
                    anchors.centerIn: parent
                    drag.target: seekBubble
                    drag.axis: Drag.XAxis
                    drag.minimumX: -40
                    drag.maximumX: width - UI.PADDING_DOUBLE
                    onExited: posInsideMouseArea = false
                    onEntered: posInsideMouseArea = true
                    onPressed: {
                        posInsideMouseArea = true;
                        seekBubble.x = mouseX - 40;
                    }
                    onReleased: if (posInsideMouseArea) videoPlayer.position = Math.floor((mouseX / width) * videoPlayer.duration);
                }
            }

            MyToolIcon {
                id: queueButton

                anchors.right: parent.right
                platformIconId: "toolbar-list"
                enabled: !videoPlayer.loading
                onClicked: {
                    dialogLoader.sourceComponent = queueDialog;
                    dialogLoader.item.open();
                }
            }
        }
    }

    Item {
        id: formatButton

        z: 10
        width: 80
        height: 60
        anchors {
            right: videoPlayer.right
            top: videoPlayer.top
            margins: UI.PADDING_DOUBLE
        }
        visible: (toolBar.show) && (!playbackQueue.data(playbackQueue.position, "streamUrl").toString())

        Rectangle {
            anchors.fill: parent
            color: "#000"
            radius: 10
            opacity: 0.5
        }

        Label {
            id: formatLabel

            anchors.centerIn: parent
        }

        MouseArea {
            anchors.fill: parent
            onClicked: {
                var service = playbackQueue.data(playbackQueue.position, "service");
                dialogLoader.sourceComponent = streamDialog;
                videoPlayer.resumePosition = videoPlayer.position;
                videoPlayer.pause();

                if (service == Resources.YOUTUBE) {
                    dialogLoader.item.model = youtubeModel;
                }
                else if (service == Resources.DAILYMOTION) {
                    dialogLoader.item.model = dailymotionModel;
                }
                else if (service == Resources.VIMEO) {
                    dialogLoader.item.model = vimeoModel;
                }
                else {
                    dialogLoader.item.model = pluginModel;
                    pluginModel.service = service;
                }

                dialogLoader.item.model.list(playbackQueue.data(playbackQueue.position, "id"));
                dialogLoader.item.open();
            }
        }
    }

    Timer {
        id: controlsTimer

        running: (toolBar.show) && (!seekMouseArea.pressed)
        interval: 3000
        onTriggered: toolBar.show = false
    }

    ScreenSaver {
        id: screenSaver
    }

    Multimedia.Video {
        id: videoPlayer

        property int resumePosition: 0

        property bool loading: (status == Multimedia.Video.Buffering) || (status == Multimedia.Video.Loading)
                               || (youtubeModel.status == 1) || (dailymotionModel.status == 1)
                               || (vimeoModel.status == 1) || (pluginModel.status == 1)

        function playUrl(url) {
            source = decodeURIComponent(url);
            play();
        }

        function toggleAspectRatio() {
            switch (fillMode) {
            case Multimedia.Video.PreserveAspectFit:
                fillMode = Multimedia.Video.PreserveAspectCrop;
                return;
            case Multimedia.Video.PreserveAspectCrop:
                fillMode = Multimedia.Video.Stretch;
                return;
            default:
                fillMode = Multimedia.Video.PreserveAspectFit;
                return;
            }
        }

        width: 854
        height: 480
        anchors.centerIn: parent
        onStarted: screenSaver.preventBlanking(true)
        onResumed: screenSaver.preventBlanking(true)
        onPaused: screenSaver.preventBlanking(false)
        onStopped: screenSaver.preventBlanking(false)
        onError: {
            if ((error !== Multimedia.Video.NoError) && (source != "")) {
                infoBanner.showMessage(qsTr("Cannot play video"));
            }
        }
        onStatusChanged: {
            switch (status) {
            case Multimedia.Video.Buffered:
                if (resumePosition > 0) {
                    position = resumePosition;
                    resumePosition = 0;
                }

                break;
            case Multimedia.Video.EndOfMedia:
                next();
                break;
            default:
                break;
            }
        }
    }

    Rectangle {
        id: videoBackground

        z: videoPlayer.loading ? 0 : -1
        anchors.fill: videoPlayer
        color: "#000"
    }

    MouseArea {
        id: videoMouseArea

        property int xPos

        anchors.fill: videoPlayer
        enabled: !videoPlayer.loading
        onPressed: xPos = mouseX
        onReleased: {
            if (((xPos - mouseX) > 100) && (playbackQueue.position < playbackQueue.count - 1)) {
                next();
            }
            else if ((mouseX - xPos) > 100) {
                previous();
            }
        }
        onClicked: if (!appWindow.inPortrait) toolBar.show = !toolBar.show;
        onDoubleClicked: videoPlayer.toggleAspectRatio()
    }

    MyBusyIndicator {
        id: busyIndicator

        size: "large"
        anchors.centerIn: videoPlayer
        visible: videoPlayer.loading
    }

    Loader {
        id: dialogLoader
    }

    Component {
        id: streamDialog

        ValueDialog {
            titleText: qsTr("Video format")
            showProgressIndicator: (model) && (model.status == 1)
            onAccepted: {
                videoPlayer.playUrl(value.url);
                formatLabel.text = value.height + "p";
                Settings.setDefaultPlaybackFormat(playbackQueue.data(playbackQueue.position, "service"), model.data(selectedIndex, "name"));
            }
            onStatusChanged: if ((status == DialogStatus.Closing) && (model.status == 1)) model.cancel();
        }
    }

    Component {
        id: queueDialog

        MySelectionDialog {
            titleText: qsTr("Playback queue")
            model: playbackQueue
            delegate: Item {
                id: delegateItem

                property bool selected: index == playbackQueue.position;

                height: root.platformStyle.itemHeight
                anchors.left: parent.left
                anchors.right: parent.right

                MouseArea {
                    id: delegateMouseArea
                    anchors.fill: parent;
                    onClicked: {
                        playbackQueue.position = index;
                        accept();
                    }
                }


                Rectangle {
                    id: backgroundRect
                    anchors.fill: parent
                    color: delegateItem.selected ? root.platformStyle.itemSelectedBackgroundColor : root.platformStyle.itemBackgroundColor
                }

                BorderImage {
                    id: background
                    anchors.fill: parent
                    border { left: UI.CORNER_MARGINS; top: UI.CORNER_MARGINS; right: UI.CORNER_MARGINS; bottom: UI.CORNER_MARGINS }
                    source: delegateMouseArea.pressed ? root.platformStyle.itemPressedBackground :
                            delegateItem.selected ? root.platformStyle.itemSelectedBackground :
                            root.platformStyle.itemBackground
                }

                Text {
                    id: itemText
                    elide: Text.ElideRight
                    color: delegateItem.selected ? root.platformStyle.itemSelectedTextColor : root.platformStyle.itemTextColor
                    anchors.verticalCenter: delegateItem.verticalCenter
                    anchors.left: parent.left
                    anchors.right: parent.right
                    anchors.leftMargin: root.platformStyle.itemLeftMargin
                    anchors.rightMargin: root.platformStyle.itemRightMargin
                    text: title;
                    font: root.platformStyle.itemFont
                }
            }
            onAccepted: getNextVideo()
        }
    }

    states: [
        State {
            name: "portrait"
            when: appWindow.inPortrait
            PropertyChanges { target: appWindow; showStatusBar: true; showToolBar: true }
            PropertyChanges { target: formatButton; visible: true }
            AnchorChanges { target: formatButton; anchors { top: undefined; bottom: videoPlayer.top } }
            PropertyChanges { target: toolBar; show: false }
            PropertyChanges { target: toolBarStyle; background: "image://theme/meegotouch-toolbar-landscape-inverted-background" }
            PropertyChanges { target: stopButton; visible: false }
            AnchorChanges { target: playButton; anchors.left: parent.left }
            PropertyChanges { target: videoPlayer; width: 480; height: 360 }
            PropertyChanges { target: progressBarBinding; value: videoPlayer.position }
            PropertyChanges { target: positionLabelBinding; value: Utils.formatMSecs(videoPlayer.position) }
        },

        State {
            name: "landscape"
            when: !appWindow.inPortrait
            PropertyChanges { target: appWindow; showStatusBar: false; showToolBar: false }
            PropertyChanges { target: appStyle; cornersVisible: false }
            PropertyChanges { target: stopButton; visible: true }
        }
    ]

    onApplicationActiveChanged: screenSaver.preventBlanking((applicationActive) && (videoPlayer.playing) && (!videoPlayer.paused))
    onStatusChanged: if (status === PageStatus.Active) startPlayback();
}
