import QtQuick 2.9
import QtQuick.Controls 2.2

import "qrc:/publicComponentes/" as Components

Components.BasePage {
    id: page
    title: qsTr("Logout")
    toolBarState: "goBack"
    absPath: Config.plugins.login + "Logout.qml"
    hasListView: false; hasNetworkRequest: false; showToolBar: !timer.running
    showTabBar: !timer.running // hide the tabBar after user confirm to exit
    lockGoBack: timer.running // disable return to previous page, after user confirm to exit

    // countdown to pop the logout page
    Timer {
        id: timer
        interval: 2000
        onTriggered: Subject.notify(Config.events.logoutUser, 0)
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

        Button {
            text: qsTr("Yes! exit now")
            onClicked: timer.running = true
            anchors.horizontalCenter: parent.horizontalCenter
        }
    }
}
