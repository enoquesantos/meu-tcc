import QtQuick.Controls 2.0

import "qrc:/publicComponentes/" as Components

Components.BasePage {
    id: page
    hasListView: false
    hasNetworkRequest: false
    title: qsTr("Page 4")
    absPath: Config.plugins.pages + "Page4.qml"

    Label {
        anchors.centerIn: parent
        text: page.title
    }
}
