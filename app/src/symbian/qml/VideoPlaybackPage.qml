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

import QtQuick 1.1
import com.nokia.symbian 1.1
import QtMultimediaKit 1.1 as Multimedia
import QtMobility.systeminfo 1.1
import cuteTube 2.0

MyPage {
    id: root

    property bool applicationActive: (Qt.application.active) && (!deviceInfo.isDeviceLocked)

    function addVideos(videos) {
        for (var i = 0; i < videos.length; i++) {
            playbackQueue.append(videos[i]);
        }

        playbackQueue.position = 0;
    }

    function startPlayback() {
        var video = playbackQueue.get(playbackQueue.position);
        title = video.title;

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
    title: qsTr("Now playing")
    tools: ToolBarLayout {

        BackToolButton {
            onClicked: {
                stopPlayback();
                appWindow.showStatusBar = true;
                appWindow.showToolBar = true;
                appWindow.cornersVisible = true;
            }
        }
    }

    Keys.onPressed: {
        switch (event.key) {
        case 112:
        case Qt.Key_Space:
            videoPlayer.setPaused = !videoPlayer.setPaused;
            break;
        case Qt.Key_Left:
            previous();
            break;
        case Qt.Key_Right:
            next();
            break;
        default:
            return;
        }

        event.accepted = true;
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

    Item {
        id: toolBar

        property bool show: false

        z: 10
        height: 60
        anchors {
            left: videoPlayer.left
            right: videoPlayer.right
            bottom: videoPlayer.bottom
            bottomMargin: show ? 0 : -height
        }

        Rectangle {
            anchors.fill: parent
            color: "#000"
            opacity: 0.5
        }

        MouseArea {
            anchors.fill: parent
            z: -1
        }

        Item {
            anchors.fill: parent

            BackToolButton {
                id: stopButton

                anchors {
                    left: parent.left
                    verticalCenter: parent.verticalCenter
                }
                onClicked: {
                    stopPlayback();
                    appWindow.showStatusBar = true;
                    appWindow.showToolBar = true;
                    appWindow.cornersVisible = true;
                }
            }

            MyToolButton {
                id: playButton

                anchors {
                    left: stopButton.right
                    verticalCenter: parent.verticalCenter
                }
                iconSource: "toolbar-mediacontrol-" + ((videoPlayer.paused) || (!videoPlayer.playing) ? "play" : "pause")
                onClicked: {
                    if (videoPlayer.loading) {
                        return;
                    }

                    if (videoPlayer.playing) {
                        videoPlayer.setPaused = !videoPlayer.setPaused;
                        controlsTimer.restart();
                    }
                    else {
                        videoPlayer.position = 0;
                        videoPlayer.play();
                    }
                }
            }

            ProgressBar {
                id: progressBar

                anchors {
                    left: playButton.right
                    leftMargin: platformStyle.paddingLarge
                    right: queueButton.left
                    rightMargin: platformStyle.paddingLarge
                    verticalCenter: parent.verticalCenter
                }
                minimumValue: 0
                maximumValue: videoPlayer.duration
                value: videoPlayer.position

                Label {
                    id: positionLabel

                    anchors {
                        top: parent.bottom
                        horizontalCenter: parent.left
                    }
                    font.pixelSize: platformStyle.fontSizeSmall
                    text: Utils.formatMSecs(videoPlayer.position)
                }

                Label {
                    id: durationLabel

                    anchors {
                        top: parent.bottom
                        horizontalCenter: parent.right
                    }
                    font.pixelSize: platformStyle.fontSizeSmall
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
                    drag.maximumX: width - platformStyle.paddingLarge
                    onExited: posInsideMouseArea = false
                    onEntered: posInsideMouseArea = true
                    onPressed: {
                        posInsideMouseArea = true;
                        seekBubble.x = mouseX - 40;
                    }
                    onReleased: if (posInsideMouseArea) videoPlayer.position = Math.floor((mouseX / width) * videoPlayer.duration);
                }
            }

            MyToolButton {
                id: queueButton

                anchors {
                    right: parent.right
                    verticalCenter: parent.verticalCenter
                }
                iconSource: "toolbar-list"
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
        width: 60
        height: 45
        anchors {
            right: videoPlayer.right
            top: videoPlayer.top
            margins: platformStyle.paddingLarge
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

            anchors {
                fill: parent
                margins: platformStyle.paddingSmall
            }
            horizontalAlignment: Text.AlignHCenter
            verticalAlignment: Text.AlignVCenter
            elide: Text.ElideRight
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

    Timer {
        id: resumeTimer

        interval: 500
        onTriggered: if ((!applicationActive) && (!videoPlayer.playing)) stopPlayback();
    }

    DeviceInfo {
        id: deviceInfo

        monitorLockStatusChanges: true
        onLockStatusChanged: if ((!isDeviceLocked) && (!videoPlayer.playing)
                                 && (videoPlayer.resumePosition > 0)) startPlayback();
    }

    VolumeControl {
        id: volumeControl

        z: 10
    }

    Multimedia.Video {
        id: videoPlayer

        property int resumePosition: -1
        property bool setPaused: false

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

        width: parent.width
        height: parent.height
        anchors.centerIn: parent
        volume: VolumeKeys.volume
        onSetPausedChanged: paused = setPaused
        onStarted: screenSaver.setScreenSaverInhibit(true)
        onResumed: screenSaver.setScreenSaverInhibit(true)
        onPaused: screenSaver.setScreenSaverInhibit(false)
        onStopped: screenSaver.setScreenSaverInhibit(false)
        onError: {
            if ((error !== Multimedia.Video.NoError) && (source != "")) {
                infoBanner.showMessage(qsTr("Cannot play video"));
            }
        }
        onStatusChanged: {
            switch (status) {
            case Multimedia.Video.Buffered:
                if (resumePosition > 0) {
                    pause();
                    position = resumePosition;
                    resumePosition = 0;

                    if (!setPaused) {
                        play();
                    }
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

    BusyIndicator {
        id: busyIndicator

        width: platformStyle.graphicSizeLarge
        height: platformStyle.graphicSizeLarge
        anchors.centerIn: videoPlayer
        visible: videoPlayer.loading
        running: visible
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
                formatLabel.text = (value.height ? value.height + "p" : value.description);
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
            delegate: MenuItem {
                text: title
                onClicked: {
                    selectedIndex = index;
                    root.accept();
                }

                Keys.onPressed: {
                    if (event.key == Qt.Key_Up || event.key == Qt.Key_Down)
                        scrollBar.flash()
                }
            }

            onAccepted: {
                playbackQueue.position = selectedIndex;
                getNextVideo();
            }
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
            PropertyChanges { target: stopButton; visible: false }
            AnchorChanges { target: playButton; anchors.left: parent.left }
            PropertyChanges { target: videoPlayer; height: width * 3 / 4 }
        },

        State {
            name: "landscape"
            when: !appWindow.inPortrait
            PropertyChanges { target: appWindow; showStatusBar: false; showToolBar: false }
            PropertyChanges { target: appWindow; cornersVisible: false }
            PropertyChanges { target: stopButton; visible: true }
            PropertyChanges { target: toolBar; height: 70 }
        }
    ]

    onStatusChanged: if (status === PageStatus.Active) startPlayback();
    onApplicationActiveChanged: {
        screenSaver.setScreenSaverInhibit((applicationActive) && (videoPlayer.playing) && (!videoPlayer.paused));

        if (applicationActive) {
            if ((!videoPlayer.playing) && (videoPlayer.resumePosition > 0)) {
                startPlayback();
            }
        }
        else {
            videoPlayer.resumePosition = videoPlayer.position;
            resumeTimer.restart();
        }
    }
}
