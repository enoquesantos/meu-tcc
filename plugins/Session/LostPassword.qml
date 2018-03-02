import QtQuick 2.8
import QtQuick.Controls 2.1

import "qrc:/src/qml/" as Components

Components.BasePage {
    id: page
    objectName: "LostPassword.qml"
    absPath: Config.plugins.session + objectName
    showTabBar: false; hasListView: false

    // handle request http responses
    onRequestFinished: {
        if (statusCode === 404)
            functions.alert(qsTr("Error!"), qsTr("Email not found. try again!"))
        else if (statusCode === 200)
            functions.alert(qsTr("OK"), qsTr("Your password was sent for your email!"))
        else
            functions.alert(qsTr("Error!"), qsTr("An error occur in the server! Try again!"))
    }

    Components.AwesomeIcon {
        z: 1; width: size*1.2; height: width
        size: 22; name: "arrow_left"; color: Config.theme.colorPrimary
        anchors { top: parent.top; topMargin: 25; left: parent.left; leftMargin: 20 }
        onClicked: pageStack.pop()
    }

    Brand {
        id: brand
    }

    Column {
        id: content
        spacing: 45
        width: parent.width * 0.90
        anchors { top: brand.bottom; topMargin: 15; horizontalCenter: parent.horizontalCenter }

        Components.PasswordField {
            id: email
            placeholderText: qsTr("Email")
            inputMethodHints: Qt.ImhEmailCharactersOnly | Qt.ImhLowercaseOnly
        }

        Button {
            enabled: !isPageBusy
            text: qsTr("Submit")
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
