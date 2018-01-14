import QtQuick 2.8

import "qrc:/src/qml/" as Components

Components.BasePage {
    id: page
    objectName: "Login.qml"
    absPath: Config.plugins.session + objectName
    centralizeBusyIndicator: false
    lockGoBack: true; showTabBar: false; showToolBar: false; hasListView: false

    // handle request http responses
    onRequestFinished: {
       if (statusCode === 403 || statusCode === 404)
           functions.alert(qsTr("Error!"), qsTr("Login or Password is invalid. Check your credentials and try again!"), null, null)
       else if (statusCode === 200)
           loginResult = response
       else
           functions.alert(qsTr("Error!"), qsTr("A error occur in the server! Try again!"), null, null)
   }

    property var loginResult: null

    // countdown to pop the login page
    Timer {
        interval: 1200; repeat: false
        running: loginResult != null
        onTriggered: App.eventNotify(Config.events.initUserProfile, loginResult)
        onRunningChanged: if (running) busyIndicator.visible = true
    }

    Flickable {
        anchors.fill: parent
        contentHeight: Math.max(content.implicitHeight, height)
        boundsBehavior: Flickable.StopAtBounds

        Components.Brand {
            id: brand
        }

        Column {
            id: content
            spacing: 25
            width: parent.width * 0.90
            anchors { top: brand.bottom; topMargin: 15; horizontalCenter: parent.horizontalCenter }

            Components.PasswordField {
                id: login
                nextFocusItem: password
                placeholderText: qsTr("Login")
            }

            Components.PasswordField {
                id: password
                nextFocusItem: loginButton
                echoMode: TextInput.Password
                inputMethodHints: Qt.ImhNoPredictiveText
                placeholderText: qsTr("Password")
                onAccepted: loginButton.clicked()
            }

            Components.CustomButton {
                id: loginButton
                enabled: !isPageBusy && loginResult === null
                text: qsTr("LOG IN")
                onClicked: {
                    if (!login.text.length)
                        functions.alert(qsTr("Error!"), qsTr("Please! Enter your login!"), null, null)
                    else if (!password.text.length)
                        functions.alert(qsTr("Error!"), qsTr("Please! Enter your password!"), null, null)
                    else
                        requestHttp.post("/login", JSON.stringify({"login":login.text,"password":password.text}))
                }
            }

            Components.CustomButton {
                id: lostPasswordButton
                usesAlternativeColor: true
                enabled: !isPageBusy && loginResult === null
                text: qsTr("Retrieve password")
                onClicked: pageStack.push(Config.plugins.session + "LostPassword.qml")
            }
        }
    }
}
