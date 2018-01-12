import QtQuick.Controls 2.0

import "qrc:/src/qml/" as Components

Components.BasePage {
    id: page
    hasListView: false
    hasNetworkRequest: false
    absPath: Config.plugins.pages + "Page1.qml"
    title: qsTr("Page 1")
    toolBarState: "goBack"

    Label {
        anchors.centerIn: parent
        text: page.title
    }
}
