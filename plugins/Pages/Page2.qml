import QtQuick 2.9
import QtQuick.Controls 2.2

import "qrc:/publicComponents/" as Components

Components.BasePage {
    id: page
    hasListView: false
    hasNetworkRequest: false
    title: qsTr("Page 2")
    absPath: Config.plugins.pages + "Page2.qml"
    toolBarButtons: [
        {
            "iconName": "search",
            "callback": function() {
                toolBarState = "search"
            }
        }
    ]

    property string searchText
    onSearchTextChanged: label.text = searchText || page.title

    // TODO
    // replace by a observer
//    Connections {
//        target: App
//        onEventNotify: if (eventName === Config.events.cancelSearch) toolBarState = "normal"
//    }

    Label {
        id: label
        anchors.centerIn: parent
        text: page.title
    }
}
