import QtQuick 2.8
import QtQuick.Controls 2.0

import "qrc:/src/qml/" as Components

Components.BasePage {
    id: page
    hasListView: false
    hasNetworkRequest: false
    absPath: Config.plugins.pages + "Page3.qml"
    title: qsTr("Welcome!")

    property var toolBarButtons: [
        {
            "iconName": "gear",
            "callback": function() {
                console.log("gear...")
            }
        },
        {
            "iconName": "calendar",
            "callback": function() {
                console.log("calendar...")
            }
        },
        {
            "iconName": "ellipsis_v",
            "submenu": [
                {
                    "text": qsTr("Show in grid"),
                    "callback": function () { console.log("Show in grid...") }
                },
                {
                    "text": qsTr("Open drawer"),
                    "callback": function () { console.log("Open drawer...") }
                }
            ]
        }
    ]

    Label {
        id: label
        anchors.centerIn: parent
        text: page.title
    }

    Button {
        text: "Go to page 1"
        anchors { top: label.bottom; topMargin: 25; horizontalCenter: parent.horizontalCenter }
        onClicked: pageStack.push(Config.plugins.pages + "Page1.qml")
    }
}
