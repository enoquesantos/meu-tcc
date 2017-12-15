import QtQuick 2.8
import QtQuick.Controls 2.1

import "qrc:/qml/" as Components
import "qrc:/qml/Awesome/" as Awesome

Components.BasePage {
    id: page
    objectName: "LostPassword.qml"
    showTabBar: false; hasListView: false

    // handle request http responses
    onRequestFinished: {
        if (statusCode === 404)
            window.alert(qsTr("Error!"), qsTr("Email not found. try again!"), null, null)
        else if (statusCode === 200)
            window.alert(qsTr("OK"), qsTr("Your password was sent for your email!"), null, null)
        else
            window.alert(qsTr("Error!"), qsTr("An error occur in the server! Try again!"), null, null)
    }

    Awesome.Icon {
        z: 1; width: size*1.2; height: width
        size: 22; name: "arrow_left"; color: Config.theme.colorPrimary
        anchors { top: parent.top; topMargin: 25; left: parent.left; leftMargin: 20 }
        onClicked: pageStack.pop()
    }

    Components.Brand {
        id: brand
    }

    Column {
        id: content
        spacing: 45
        width: parent.width * 0.90
        anchors { top: brand.bottom; topMargin: 15; horizontalCenter: parent.horizontalCenter }

        Components.CustomTextField {
            id: email
            placeholderText: qsTr("Email")
            inputMethodHints: Qt.ImhEmailCharactersOnly | Qt.ImhLowercaseOnly
        }

        Components.CustomButton {
            enabled: !isPageBusy
            text: qsTr("Submit")
            onClicked: {
                var regex = /^(([^<>()\[\]\\.,;:\s@"]+(\.[^<>()\[\]\\.,;:\s@"]+)*)|(".+"))@((\[[0-9]{1,3}\.[0-9]{1,3}\.[0-9]{1,3}\.[0-9]{1,3}])|(([a-zA-Z\-0-9]+\.)+[a-zA-Z]{2,}))$/
                if (!email.text)
                    window.alert(qsTr("Error!"), qsTr("Enter your Email!"), null, null)
                else if (!regex.test(email.text))
                    window.alert(qsTr("Error!"), qsTr("Enter a valid Email!"), null, null)
                else
                    requestHttp.get("/recoverPassword/" + email.text + "/")
            }
        }
    }
}
