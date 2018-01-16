import QtQuick 2.8
import QtQuick.Layouts 1.3
import QtQuick.Controls 2.1

import "qrc:/src/qml/" as Components

Components.BasePage {
    id: page
    title: qsTr("My profile")
    hasNetworkRequest: false
    absPath: Config.plugins.session + "ProfileView.qml"
    listViewDelegate: Components.ListItem {
        showSeparator: true
        primaryLabelText: field
        secondaryLabelText: value
        primaryIconName: awesomeIcon
    }

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
            width: 32; height: width
            name: "pencil"; size: 22
            color: Config.theme.colorAccent
            onClicked: pageStack.push(Config.plugins.session + "ProfileEdit.qml")
            anchors { top: parent.top; topMargin: 35; right: parent.right; rightMargin: 20 }
        }

        Components.RoundedImage {
            id: userImageProfile
            width: 100; height: width
            borderColor: Config.theme.colorAccent
            imgSource: userProfile.profile.image_url || "qrc:/default_user_image.svg"
            anchors.centerIn: parent
        }
    }
}
