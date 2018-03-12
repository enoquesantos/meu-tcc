import QtQuick 2.9
import QtQuick.Controls 2.2

import "qrc:/publicComponentes/" as Components

Components.BasePage {
    id: page
    absPath: Config.plugins.login + "Login.qml"
    lockGoBack: true; showTabBar: false; showToolBar: false; hasListView: false

    // handle request http responses
    onRequestFinished: {
        if (statusCode === 400 || statusCode === 403 || statusCode === 404) {
            functions.alert(qsTr("Error!"), qsTr("Login or Password is invalid. Check your credentials and try again!"))
        } else if (statusCode === 200) {
            var _response = response.user
            _response.enrollment = response.enrollment
            _response.program = response.program
            _response.permission.name = _response.permission.description
            loginResult = _response
        } else {
            functions.alert(qsTr("Error!"), qsTr("A error occur in the server! Try again!"))
        }
    }

    property var loginResult: null

    // countdown to pop the login page
    Timer {
        interval: 1500
        running: loginResult != null
        onTriggered: Subject.notify(Config.events.initUserProfile, loginResult)
        onRunningChanged: if (running) busyIndicator.visible = true
    }

    Flickable {
        anchors.fill: parent
        contentHeight: Math.max(content.implicitHeight, height)
        boundsBehavior: Flickable.StopAtBounds

        Brand {
            id: brand
        }

        Column {
            id: content
            spacing: 25
            width: parent.width * 0.90
            anchors { top: brand.bottom; topMargin: 15; horizontalCenter: parent.horizontalCenter }

            TextField {
                id: login
                color: Config.theme.textColorPrimary
                selectByMouse: true; cursorVisible: focus
                renderType: Text.NativeRendering
                anchors.horizontalCenter: parent.horizontalCenter
                placeholderText: qsTr("Login")
                onAccepted: password.focus = true
                onEditingFinished: text = text.trim()
            }

            Components.PasswordField {
                id: password
                nextFocusItem: loginButton
                echoMode: TextInput.Password
                inputMethodHints: Qt.ImhNoPredictiveText
                placeholderText: qsTr("Password")
                onAccepted: loginButton.clicked()
            }

            Button {
                id: loginButton
                width: page.width * 0.50
                enabled: !isPageBusy && loginResult === null
                anchors.horizontalCenter: parent.horizontalCenter
                text: qsTr("LOG IN")
                onClicked: {
                    if (!login.text.length)
                        functions.alert(qsTr("Error!"), qsTr("Please! Enter your login!"))
                    else if (!password.text.length)
                        functions.alert(qsTr("Error!"), qsTr("Please! Enter your password!"))
                    else
                        requestHttp.post("/login/", JSON.stringify({"login":login.text,"password":password.text}))
                }
            }

            Button {
                id: lostPasswordButton
                flat: true
                width: page.width * 0.50
                anchors.horizontalCenter: parent.horizontalCenter
                enabled: !isPageBusy && loginResult === null
                text: qsTr("Retrieve password")
                onClicked: pageStack.push(Config.plugins.login + "LostPassword.qml")
            }
        }
    }
}
