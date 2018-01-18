import QtQuick.Controls 2.0

import "qrc:/publicComponentes/" as Components

Components.BasePage {
    id: page
    hasListView: false
    hasNetworkRequest: true
    absPath: Config.plugins.pages + "Page1.qml"
    title: qsTr("Page 1")
    toolBarState: "goBack"
    onRequestFinished: {
        console.log("RequestFinished! \n")
        console.log("statusCode: ", statusCode)
        console.log("response.data: ", JSON.stringify(response))
    }

    Label {
        id: label
        anchors.centerIn: parent
        text: page.title
    }

    Button {
        text: "Open a request http"
        anchors { top: label.bottom; topMargin: 25; horizontalCenter: parent.horizontalCenter }
        onClicked: requestHttp.get("url-here")
    }
}
