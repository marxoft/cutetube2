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
import com.nokia.meego 1.0
import cuteTube 2.0
import ".."
import "file:///usr/lib/qt4/imports/com/nokia/meego/UIConstants.js" as UI

MyPage {
    id: root
    
    property alias service: plugin.pluginId
    property alias settings: repeater.model

    title: qsTr("Plugin settings")
    tools: ToolBarLayout {
        BackToolIcon {}
    }
    
    PluginSettings {
        id: plugin
    }
    
    Flickable {
        id: flickable
        
        anchors.fill: parent
        contentHeight: column.height + UI.PADDING_DOUBLE
        
        Column {
            id: column
            
            anchors {
                left: parent.left
                right: parent.right
                top: parent.top
                margins: UI.PADDING_DOUBLE
            }
            spacing: UI.PADDING_DOUBLE
            
            Repeater {
                id: repeater
                
                Loader {
                    function initSourceComponent() {
                        switch (modelData.type) {
                        case "boolean":
                            sourceComponent = checkBox;
                            break;
                        case "group":
                            sourceComponent = group;
                            break;
                        case "integer":
                            sourceComponent = integerField;
                            break;
                        case "list":
                            sourceComponent = valueSelector;
                            break;
                        case "password":
                            sourceComponent = passwordField;
                            break;
                        case "text":
                            sourceComponent = textField;
                            break;
                        default:
                            break;
                        }

                        if (item) {
                            item.init(modelData);
                        }
                    }

                    Component.onCompleted: initSourceComponent()
                }
            }
        }
        
        ScrollDecorator {
            flickableItem: flickable
        }
    }
    
    Component {
        id: checkBox
        
        MySwitch {
            property string key

            function init(modelData, group) {
                key = (group ? group + "/" : "") + modelData.key;
                text = modelData.label;
                checked = plugin.value(key, modelData.value) === true;
            }

            width: column.width
            onCheckedChanged: plugin.setValue(key, checked)
        }
    }
    
    Component {
        id: group
        
        Column {            
            function init(modelData) {
                label.text = modelData.label;
                repeater.key = modelData.key;
                repeater.model = modelData.settings;
            }

            width: column.width
            spacing: UI.PADDING_DOUBLE
            
            SeparatorLabel {
                id: label

                width: parent.width
            }
            
            Repeater {
                id: repeater

                property string key
                
                Loader {
                    function initSourceComponent() {
                        switch (modelData.type) {
                        case "boolean":
                            sourceComponent = checkBox;
                            break;
                        case "integer":
                            sourceComponent = integerField;
                            break;
                        case "list":
                            sourceComponent = valueSelector;
                            break;
                        case "password":
                            sourceComponent = passwordField;
                            break;
                        case "text":
                            sourceComponent = textField;
                            break;
                        default:
                            break;
                        }

                        if (item) {
                            item.init(modelData, repeater.key);
                        }
                    }

                    Component.onCompleted: initSourceComponent()
                }
            }
        }
    }
    
    Component {
        id: integerField
        
        Column {
            function init(modelData, group) {
                label.text = modelData.label;
                field.key = (group ? group + "/" : "") + modelData.key;
                field.validator.bottom = Math.max(0, parseInt(modelData.minimum));
                field.validator.top = Math.max(1, parseInt(modelData.maximum));
                field.text = plugin.value(field.key, modelData.value);
            }

            width: column.width
            spacing: UI.PADDING_DOUBLE
            
            Label {
                id: label

                width: parent.width
                font.bold: true
                elide: Text.ElideRight
            }
            
            MyTextField {
                id: field

                property string key

                width: parent.width
                validator: IntValidator {}
                inputMethodHints: Qt.ImhDigitsOnly
                onTextChanged: plugin.setValue(key, text)
                onAccepted: platformCloseSoftwareInputPanel()
            }
        }
    }
    
    Component {
        id: passwordField
        
        Column {
            function init(modelData, group) {
                label.text = modelData.label;
                field.key = (group ? group + "/" : "") + modelData.key;
                field.text = plugin.value(field.key, modelData.value);
            }

            width: column.width
            spacing: UI.PADDING_DOUBLE
            
            Label {
                id: label

                width: parent.width
                font.bold: true
                elide: Text.ElideRight
            }
            
            MyTextField {
                id: field

                property string key

                width: parent.width
                echoMode: TextInput.Password
                inputMethodHints: Qt.ImhNoAutoUppercase | Qt.ImhNoPredictiveText
                onTextChanged: plugin.setValue(key, text)
                onAccepted: platformCloseSoftwareInputPanel()
            }
        }
    }
    
    Component {
        id: textField
        
        Column {
            function init(modelData, group) {
                label.text = modelData.label;
                field.key = (group ? group + "/" : "") + modelData.key;
                field.text = plugin.value(field.key, modelData.value);
            }

            width: column.width
            spacing: UI.PADDING_DOUBLE
            
            Label {
                id: label

                width: parent.width
                font.bold: true
                elide: Text.ElideRight
            }
            
            MyTextField {
                id: field

                property string key

                width: parent.width
                inputMethodHints: Qt.ImhNoAutoUppercase | Qt.ImhNoPredictiveText
                onTextChanged: plugin.setValue(key, text)
                onAccepted: platformCloseSoftwareInputPanel()
            }
        }
    }
    
    Component {
        id: valueSelector
        
        ValueSelector {
            property string key

            function init(modelData, group) {
                key = (group ? group + "/" : "") + modelData.key
                title = modelData.label;

                for (var i = 0; i < modelData.options.length; i++) {
                    var option = modelData.options[i];
                    model.append(option.label, option.value);
                }

                value = plugin.value(key, modelData.value);
            }

            x: -UI.PADDING_DOUBLE
            width: column.width + UI.PADDING_DOUBLE * 2
            model: SelectionModel {}
            onAccepted: plugin.setValue(key, value)
        }
    }
}
