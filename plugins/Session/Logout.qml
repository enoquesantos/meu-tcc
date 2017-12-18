import QtQuick 2.8
import QtQuick.Controls 2.1

import "qrc:/src/qml/" as Components

Components.BasePage {
    title: qsTr("Logout")
    objectName: "Logout.qml"
    toolBarState: "goback"
    hasListView: false; hasNetworkRequest: false; showToolBar: !timer.running
    // hide the tabBar after user confirm to exit
    showTabBar: !timer.running
    // disable return after confirm to exit
    lockGoBack: timer.running

    // countdown to pop the logout page
    Timer {
        id: timer
        interval: 5000
        onTriggered: user.setProfile(false)
    }

    BusyIndicator {
        visible: timer.running
        anchors.centerIn: parent
    }

    Label {
        id: label
        visible: timer.running
        text: qsTr("Good bye!")
        color: Config.theme.colorPrimary
        opacity: 0.7; font { pointSize: Config.fontSize.large; bold: true }
        anchors { bottom: parent.bottom; bottomMargin: 20; horizontalCenter: parent.horizontalCenter }
    }

    Column {
        id: actionColumn
        anchors.centerIn: parent
        spacing: 50; visible: !timer.running

        Label {
            text: qsTr("Are sure you want to quit the app?")
            color: Config.theme.colorPrimary
            font { pointSize: Config.fontSize.large; bold: true }
            anchors.horizontalCenter: parent.horizontalCenter
        }

        Components.CustomButton {
            text: qsTr("Yes! exit now")
            onClicked: timer.running = true
            anchors.horizontalCenter: parent.horizontalCenter
        }
    }
}
