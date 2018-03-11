import QtQuick.Controls 2.2

import "qrc:/publicComponentes/" as Components

Components.BasePage {
    id: page
    hasListView: false
    hasNetworkRequest: false
    title: qsTr("Page 6")
    absPath: Config.plugins.pages + "Page6.qml"

    Label {
        anchors.centerIn: parent
        text: page.title
    }
}
