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
        console.log("----------------------------")
        console.log("signal::RequestFinished...\n")
        console.log("statusCode: ", statusCode)
        console.log("response: ", JSON.stringify(response))
    }

    function arrayCallback(status, result) {
        console.log("----------------------------")
        console.log("arrayCallback!")
        console.log("status is: " + status)
        console.log("result is: " + result[0].body)
    }

    function objcCallback(status, result) {
        console.log("----------------------------")
        console.log("objcCallback!")
        console.log("status is: " + status)
        console.log("result is: " + result.body)
    }

    Label {
        id: label
        anchors.centerIn: parent
        text: page.title
    }

    Button {
        text: "Open a request http"
        anchors { top: label.bottom; topMargin: 25; horizontalCenter: parent.horizontalCenter }
        onClicked: {
            requestHttp.get("https://jsonplaceholder.typicode.com/posts/1/comments")
            requestHttp.get("https://jsonplaceholder.typicode.com/posts", {}, {}, arrayCallback)
            requestHttp.get("https://jsonplaceholder.typicode.com/posts/1", {}, {}, objcCallback)
        }
    }
}
