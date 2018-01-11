import QtQuick.Controls 2.0

import "qrc:/src/qml/" as Components

Components.BasePage {
    id: page
    hasListView: false
    hasNetworkRequest: false
    title: qsTr("Page 5")

    Label {
        anchors.centerIn: parent
        text: page.title
    }
}
