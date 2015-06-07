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

import QtQuick 2.0
import QtQuick.Controls 1.1
import QtQuick.Layouts 1.1
import QtQuick.Dialogs 1.2
import cuteTube 2.0

MyDialog {
    id: root
    
    minimumWidth: 600
    minimumHeight: 400
    title: qsTr("Preferences")
    content: TabView {
        id: tabs
        
        anchors.fill: parent
        
        Tab {
            id: generalTab
            
            title: qsTr("General")
            
            GridLayout {
                anchors {
                    left: parent.left
                    right: parent.right
                    top: parent.top
                    margins: 10
                }
                columns: 3
                columnSpacing: 10
                rowSpacing: 10
                
                Label {
                    text: qsTr("Default download path") + ":"
                }
                
                TextField {
                    id: downloadPathField
                    
                    Layout.fillWidth: true
                    text: Settings.downloadPath
                }
                
                Button {
                    id: browseButton
                    
                    text: qsTr("Browse")
                    iconName: "document-open"
                    onClicked: {
                        loader.sourceComponent = fileDialog;
                        loader.item.open();
                    }
                }
                
                CheckBox {
                    id: transfersCheckBox
                    
                    Layout.columnSpan: 3
                    text: qsTr("Start transfers automatically")
                    checked: Settings.startTransfersAutomatically
                    onCheckedChanged: Settings.startTransfersAutomatically = checked
                }
                
                Label {
                    text: qsTr("Language filter") + ":"
                }
                
                ComboBox {
                    id: localeSelector
                    
                    Layout.fillWidth: true
                    model: LocaleModel {
                        id: localeModel
                    }
                    textRole: "name"
                    currentIndex: localeModel.match("value", Settings.locale)
                    onActivated: Settings.locale = localeModel.data(index, "value")
                }
                
                Item {
                    Layout.minimumWidth: browseButton.width
                }
                
                Label {
                    text: qsTr("Video player") + ":"
                }
                
                ComboBox {
                    id: videoPlayerSelector
                    
                    Layout.fillWidth: true
                    model: VideoPlayerModel {
                        id: videoPlayerModel
                    }
                    textRole: "name"
                    currentIndex: videoPlayerModel.match("value", Settings.videoPlayer)
                    onActivated: Settings.videoPlayer = videoPlayerModel.data(index, "value")
                }
                
                Item {
                    Layout.minimumWidth: browseButton.width
                }
                
                Label {
                    text: qsTr("Video player command") + ":"
                }
                
                TextField {
                    id: commandField
                    
                    Layout.fillWidth: true
                    placeholderText: qsTr("Custom command")
                    text: Settings.videoPlayerCommand
                    enabled: Settings.videoPlayer == "other"
                    onTextChanged: Settings.videoPlayerCommand = text
                }
                
                CheckBox {
                    id: safeSearchCheckBox
                    
                    Layout.columnSpan: 3
                    text: qsTr("Enable safe search")
                    checked: Settings.safeSearchEnabled
                    onCheckedChanged: Settings.safeSearchEnabled = checked
                }
                
                CheckBox {
                    id: clipboardCheckBox
                    
                    Layout.columnSpan: 3
                    text: qsTr("Monitor clipboard for URLs")
                    checked: Settings.clipboardMonitorEnabled
                    onCheckedChanged: Settings.clipboardMonitorEnabled = checked
                }
                
                Component {
                    id: fileDialog
        
                    FileDialog {
                        folder: Settings.downloadPath
                        selectFolder: true
                        title: qsTr("Default download path")
                        onAccepted: downloadPathField.text = folder.toString().substring(7)
                    }
                }
            }
        }
        
        Tab {
            id: categoriesTab
            
            title: qsTr("Categories")
            
            GridLayout {
                anchors {
                    fill: parent
                    margins: 10
                }
                columns: 3
                columnSpacing: 10
                rowSpacing: 10
                
                TableView {
                    id: catgoriesView
                
                    Layout.columnSpan: 3
                    Layout.fillWidth: true
                    model: CategoryModel {
                        id: categoryModel
                    }
                
                    TableViewColumn {
                        role: "name"
                        title: qsTr("Name")
                        width: 200
                    }
                
                    TableViewColumn {
                        role: "path"
                        title: qsTr("Download path")
                    }
                    
                    onActivated: {
                        nameField.text = categoryModel.data(row, "name");
                        pathField.text = categoryModel.data(row, "path");
                    }
                    onClicked: {
                        nameField.text = categoryModel.data(row, "name");
                        pathField.text = categoryModel.data(row, "path");
                    }
                }
                
                Label {
                    Layout.columnSpan: 3
                    text: qsTr("Add/edit category")
                }
                
                Label {
                    text: qsTr("Name") + ":"
                }
                
                TextField {
                    id: nameField              
                    
                    Layout.fillWidth: true      
                }
                
                Item {
                    Layout.minimumWidth: browseButton.width
                }
                                
                Label {
                    text: qsTr("Download path") + ":"
                }
                                
                TextField {
                    id: pathField
                    
                    Layout.fillWidth: true
                }
                
                Button {
                    id: browseButton
                    
                    text: qsTr("Browse")
                    iconName: "document-open"
                    enabled: nameField.text != ""
                    onClicked: {
                        loader.sourceComponent = fileDialog;
                        loader.item.folder = pathField.text;
                        loader.item.open();
                    }
                }
                
                Button {          
                    id: saveButton
                              
                    text: qsTr("Save")
                    iconName: "document-save"
                    enabled: (nameField.text) && (pathField.text)
                    onClicked: {
                        categoryModel.addCategory(nameField.text, pathField.text);
                        nameField.text = "";
                        pathField.text = "";
                    }
                }
                
                Component {
                    id: fileDialog
    
                    FileDialog {
                        selectFolder: true
                        title: qsTr("Download path")
                        onAccepted: pathField.text = folder.toString().substring(7)
                    }
                }
            }            
        }
        
        Tab {
            id: networkTab
            
            title: qsTr("Network")
            
            GroupBox {
                anchors {
                    fill: parent
                    margins: 10
                }
                title: qsTr("Enable network proxy")
                checkable: true
                checked: Settings.networkProxyEnabled
                onCheckedChanged: Settings.networkProxyEnabled = checked
            
                GridLayout {
                    anchors {
                        left: parent.left
                        right: parent.right
                        top: parent.top
                        topMargin: 10
                    }
                    columns: 4
                    columnSpacing: 10
                    rowSpacing: 10
                    
                    Label {
                        text: qsTr("Host") + ":"
                    }
                    
                    TextField {
                        id: proxyHostField
                        
                        Layout.fillWidth: true
                        text: Settings.networkProxyHost
                        onTextChanged: Settings.networkProxyHost = text
                    }
                        
                    Label {
                        text: qsTr("Port") + ":"
                    }
                    
                    TextField {
                        id: proxyPortField
                        
                        Layout.fillWidth: true
                        validator: IntValidator {
                            bottom: 0
                            top: 100000
                        }
                        text: Settings.networkProxyPort
                        onTextChanged: Settings.networkProxyPort = parseInt(text)
                    }
                    
                    Label {
                        text: qsTr("Username") + ":"
                    }
                    
                    TextField {
                        id: proxyUsernameField
                        
                        Layout.alignment: Qt.AlignTop
                        Layout.fillWidth: true
                        text: Settings.networkProxyUsername
                        onTextChanged: Settings.networkProxyUsername = text
                    }
                    
                    Label {
                        text: qsTr("Password") + ":"
                    }
                    
                    TextField {
                        id: proxyPasswordField
                        
                        Layout.alignment: Qt.AlignTop
                        Layout.fillWidth: true
                        echoMode: TextInput.Password
                        text: Settings.networkProxyPassword
                        onTextChanged: Settings.networkProxyPassword = text
                    }
                }
            }
        }
        
        Tab {
            id: pluginsTab
            
            title: qsTr("Plugins")
            
            Item {
                anchors {
                    fill: parent
                    margins: 10
                }

                function loadSettings(pluginName, fileName) {
                    clearLoaders();
                    
                    var request = new XMLHttpRequest();
                    request.onreadystatechange = function() {
                        if (request.readyState === XMLHttpRequest.DONE) {
                            var doc = request.responseXML.documentElement;

                            for (var i = 0; i < doc.childNodes.length; i++) {
                                var node = doc.childNodes[i];

                                if (node.nodeName === "group") {
                                    addGroup(node.attributes[0].value);

                                    for (var ii = 0; ii < node.childNodes.length; ii++) {
                                        var groupNode = node.childNodes[ii];

                                        if (groupNode.nodeName === "list") {
                                            var key = pluginName + "/" + findAttributeValue(groupNode, "key");
                                            var defaultValue = findAttributeValue(groupNode, "default");
                                            var title = findAttributeValue(groupNode, "title");
                                            var list = [];

                                            for (var iii = 0; iii < groupNode.childNodes.length; iii++) {
                                                var listNode = groupNode.childNodes[iii];

                                                if (listNode.nodeName === "element") {
                                                    var name = findAttributeValue(listNode, "name");
                                                    var value = findAttributeValue(listNode, "value");
                                                    var element = { "name": name, "value": value };
                                                    list.push(element);
                                                }
                                            }

                                            addComboBox(key, defaultValue, title, list);
                                        }
                                        else if (groupNode.nodeName === "boolean") {
                                            var key = pluginName + "/" + findAttributeValue(groupNode, "key");
                                            var defaultValue = findAttributeValue(groupNode, "default");
                                            var title = findAttributeValue(groupNode, "title");
                                            addCheckBox(key, defaultValue, title);
                                        }
                                        else if (groupNode.nodeName === "integer") {
                                            var key = pluginName + "/" + findAttributeValue(groupNode, "key");
                                            var defaultValue = findAttributeValue(groupNode, "default");
                                            var title = findAttributeValue(groupNode, "title");
                                            var min = parseInt(findAttributeValue(groupNode, "min"));
                                            var max = parseInt(findAttributeValue(groupNode, "max"));
                                            var step = parseInt(findAttributeValue(groupNode, "step"));
                                            addSlider(key, defaultValue, title, min, max, step);
                                        }
                                        else if (groupNode.nodeName === "text") {
                                            var key = pluginName + "/" + findAttributeValue(groupNode, "key");
                                            var defaultValue = findAttributeValue(groupNode, "default");
                                            var title = findAttributeValue(groupNode, "title");
                                            addTextField(key, defaultValue, title);
                                        }
                                    }
                                }
                                
                                if (node.nodeName === "list") {
                                    var key = pluginName + "/" + findAttributeValue(node, "key");
                                    var defaultValue = findAttributeValue(node, "default");
                                    var title = findAttributeValue(node, "title");
                                    var list = [];

                                    for (var iii = 0; iii < node.childNodes.length; iii++) {
                                        var listNode = node.childNodes[iii];

                                        if (listNode.nodeName === "element") {
                                            var name = findAttributeValue(listNode, "name");
                                            var value = findAttributeValue(listNode, "value");
                                            var element = { "name": name, "value": value };
                                            list.push(element);
                                        }
                                    }

                                    addComboBox(key, defaultValue, title, list);
                                }
                                else if (node.nodeName === "boolean") {
                                    var key = pluginName + "/" + findAttributeValue(node, "key");
                                    var defaultValue = findAttributeValue(node, "default");
                                    var title = findAttributeValue(node, "title");
                                    addCheckBox(key, defaultValue, title);
                                }
                                else if (node.nodeName === "integer") {
                                    var key = pluginName + "/" + findAttributeValue(node, "key");
                                    var defaultValue = findAttributeValue(node, "default");
                                    var title = findAttributeValue(node, "title");
                                    var min = parseInt(findAttributeValue(node, "min"));
                                    var max = parseInt(findAttributeValue(node, "max"));
                                    var step = parseInt(findAttributeValue(node, "step"));
                                    addSlider(key, defaultValue, title, min, max, step);
                                }
                                else if (node.nodeName === "text") {
                                    var key = pluginName + "/" + findAttributeValue(node, "key");
                                    var defaultValue = findAttributeValue(node, "default");
                                    var title = findAttributeValue(node, "title");
                                    addTextField(key, defaultValue, title);
                                }
                            }
                        }
                    }

                    request.open("GET", fileName);
                    request.send();
                }

                function findAttributeValue(node, name) {
                    for (var i = 0; i < node.attributes.length; i++) {
                        var att = node.attributes[i];

                        if (att.name === name) {
                            return att.value;
                        }
                    }

                    return "";
                }

                function findEmptyLoader() {
                    for (var i = 1; i < column.children.length; i++) {
                        var child = column.children[i];

                        if ((child.hasOwnProperty("item")) && (!child.item)) {
                            return child;
                        }
                    }
                }
                
                function clearLoaders() {
                    for (var i = 1; i < column.children.length; i++) {
                        var child = column.children[i];

                        if (child.hasOwnProperty("sourceComponent")) {
                            child.sourceComponent = undefined;
                        }
                    }
                }

                function addGroup(title) {
                    var loader = findEmptyLoader();

                    if (loader) {
                        loader.source = Qt.resolvedUrl("plugins/PluginSettingsGroupLabel.qml");
                        loader.item.text = title;
                    }
                }

                function addComboBox(key, defaultValue, title, list) {
                    var loader = findEmptyLoader();

                    if (loader) {
                        loader.source = Qt.resolvedUrl("plugins/PluginSettingsComboBox.qml");
                        loader.item.title = title;
                        loader.item.setList(key, defaultValue, list);
                    }
                }

                function addCheckBox(key, defaultValue, title) {
                    var loader = findEmptyLoader();

                    if (loader) {
                        loader.source = Qt.resolvedUrl("plugins/PluginSettingsCheckBox.qml");
                        loader.item.text = title;
                        loader.item.setKey(key, defaultValue);
                    }
                }

                function addSlider(key, defaultValue, title, min, max, step) {
                    var loader = findEmptyLoader();

                    if (loader) {
                        loader.source = Qt.resolvedUrl("plugins/PluginSettingsSlider.qml");
                        loader.item.title = title;
                        loader.item.minimumValue = min;
                        loader.item.maximumValue = max;
                        loader.item.stepSize = step;
                        loader.item.setKey(key, defaultValue);
                    }
                }

                function addTextField(key, defaultValue, title) {
                    var loader = findEmptyLoader();

                    if (loader) {
                        loader.source = Qt.resolvedUrl("plugins/PluginSettingsTextField.qml");
                        loader.item.title = title;
                        loader.item.setKey(key, defaultValue);
                    }
                }
                
                ItemView {
                    id: view
                    
                    width: 150
                    anchors {
                        left: parent.left
                        top: parent.top
                        bottom: parent.bottom
                    }
                    model: PluginSettingsModel {
                        id: pluginModel
                    }
                    delegate: LabelDelegate {
                        text: name
                    }
                    onCurrentIndexChanged: loadSettings(pluginModel.data(currentIndex, "name"),
                                                        pluginModel.data(currentIndex, "value"))
                }
                
                ScrollView {
                    anchors {
                        left: view.right
                        leftMargin: 10
                        right: parent.right
                        top: parent.top
                        bottom: parent.bottom
                    }
                    
                    Flickable {
                        anchors.fill: parent
                        contentHeight: column.height
                        
                        Column {
                            id: column
                            
                            anchors {
                                left: parent.left
                                right: parent.right
                                top: parent.top
                                topMargin: 10
                            }
                            spacing: 10
                            
                            Repeater {
                                model: 20
                                
                                Loader {
                                    width: column.width
                                }
                            }
                        }
                    }
                }
            }   
        }
    }
    
    buttons: Button {
        text: qsTr("&Ok")
        iconName: "dialog-ok"
        isDefault: true
        onClicked: root.accept();
    }
    
    Loader {
        id: loader
    }
        
    onVisibleChanged: tabs.currentIndex = 0
}
