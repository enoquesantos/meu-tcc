import QtQuick.Controls 2.2

import "qrc:/publicComponents/" as Components

Components.BasePage {
    id: page
    hasListView: false
    hasNetworkRequest: false
    title: qsTr("Page 5")
    absPath: Config.plugins.pages + "Page5.qml"

    Label {
        anchors.centerIn: parent
        text: page.title
    }
}
