import QtQuick 2.9
import QtQuick.Layouts 1.3
import QtQuick.Controls 2.2

import "qrc:/publicComponentes/" as Components

Components.BasePage {
    id: page
    title: qsTr("My profile")
    hasNetworkRequest: false
    absPath: Config.plugins.userprofile + "ProfileView.qml"
    listViewDelegate: Components.ListItem {
        showSeparator: true
        primaryLabelText: field
        secondaryLabelText: value
        primaryIconName: awesomeIcon
    }
    toolBarButtons: [
        {
            "iconName": "pencil",
            "callback": function() {
                pageStack.push(Config.plugins.userprofile + "ProfileEdit.qml")
            }
        }
    ]

    Component.onCompleted: {
        listView.anchors.topMargin = rec.height
        for (var i = 0; i < fields.length; ++i)
            listViewModel.append(fields[i])
    }

    property var fields: [
        {
            "field": qsTr("Name"),
            "value": userProfile.profile.name,
            "awesomeIcon": "user"
        },
        {
            "field": qsTr("Email"),
            "value": userProfile.profile.email,
            "awesomeIcon": "envelope"
        }
    ]

    Rectangle {
        id: rec
        width: parent.width; height: 180; color: Config.theme.colorPrimary
        anchors { top: parent.top; topMargin: 0; horizontalCenter: parent.horizontalCenter }

        Components.AwesomeIcon {
            width: 32
            name: "pencil"; size: 22
            visible: Config.usesSwipeView
            color: Config.theme.colorAccent
            onClicked: pageStack.push(Config.plugins.userprofile + "ProfileEdit.qml")
            anchors { top: parent.top; topMargin: 35; right: parent.right; rightMargin: 20 }
        }

        Components.RoundedImage {
            width: 100; height: width
            borderColor: Config.theme.colorAccent
            imgSource: userProfile.profile.image_path
            anchors.centerIn: parent
        }
    }
}
