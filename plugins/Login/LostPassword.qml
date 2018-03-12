import QtQuick 2.9
import QtQuick.Controls 2.2

import "qrc:/publicComponentes/" as Components

Components.BasePage {
    id: page
    absPath: Config.plugins.login + "LostPassword.qml"
    showTabBar: false; hasListView: false

    // handle request http responses
    onRequestFinished: {
        if (statusCode === 404)
            functions.alert(qsTr("Error!"), qsTr("Email not found. try again!"))
        else if (statusCode === 200)
            functions.alert(qsTr("OK!"), qsTr("Your password was sent for your email!"))
        else
            functions.alert(qsTr("Error!"), qsTr("An error occur in the server! Try again!"))
    }

    Components.AwesomeIcon {
        z: 2; width: 50
        size: 25; name: "arrow_left"; color: Config.theme.colorPrimary
        anchors { top: parent.top; topMargin: 25; left: parent.left; leftMargin: 2 }
        onClicked: Subject.notify(Config.events.popCurrentPage, 0)
    }

    Brand {
        id: brand
    }

    Column {
        id: content
        spacing: 45
        width: parent.width * 0.90
        anchors { top: brand.bottom; topMargin: 15; horizontalCenter: parent.horizontalCenter }

        TextField {
            id: email
            color: Config.theme.textColorPrimary
            selectByMouse: true; cursorVisible: focus
            renderType: Text.NativeRendering
            anchors.horizontalCenter: parent.horizontalCenter
            placeholderText: qsTr("Email")
            inputMethodHints: Qt.ImhEmailCharactersOnly | Qt.ImhLowercaseOnly
            onEditingFinished: text = text.trim()
        }

        Button {
            enabled: !isPageBusy
            text: qsTr("Submit")
            width: page.width * 0.40
            anchors.horizontalCenter: parent.horizontalCenter
            onClicked: {
                var regex = /^(([^<>()\[\]\\.,;:\s@"]+(\.[^<>()\[\]\\.,;:\s@"]+)*)|(".+"))@((\[[0-9]{1,3}\.[0-9]{1,3}\.[0-9]{1,3}\.[0-9]{1,3}])|(([a-zA-Z\-0-9]+\.)+[a-zA-Z]{2,}))$/
                if (!email.text)
                    functions.alert(qsTr("Error!"), qsTr("Enter your Email!"))
                else if (!regex.test(email.text))
                    functions.alert(qsTr("Error!"), qsTr("Enter a valid Email!"))
                else
                    requestHttp.get("/recoverPassword/%1/".arg(email.text))
            }
        }
    }
}
