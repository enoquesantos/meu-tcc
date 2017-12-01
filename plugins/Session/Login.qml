import QtQuick 2.8

import "qrc:/qml/"

BasePage {
    id: page
    objectName: "Login.qml"
    lockGoBack: true
    showTabBar: false
    showToolBar: false
    hasListView: false
    centralizeBusyIndicator: false

    // handle request http responses
    onRequestFinished: {
       if (statusCode === 403 || statusCode === 404) {
           window.alert(qsTr("Error!"), qsTr("Login or Password is invalid. Check your credentials and try again!"), null, null)
       } else if (statusCode === 200) {
           loginButton.enabled = lostPasswordButton.enabled = false
           loginResult = response
       } else {
           window.alert(qsTr("Error!"), qsTr("A error occur in the server! Try again!"), null, null)
       }
   }

    property var loginResult: ({})

    // countdown to pop the login page
    Timer {
        interval: 1200; repeat: false
        running: "user" in loginResult
        onTriggered: user.setProfile(loginResult)
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

            CustomTextField {
                id: login
                nextFocusItem: password
                placeholderText: qsTr("Login")
            }

            CustomTextField {
                id: password
                nextFocusItem: loginButton
                echoMode: TextInput.Password
                inputMethodHints: Qt.ImhNoPredictiveText
                placeholderText: qsTr("Password")
                onAccepted: loginButton.clicked()
            }

            CustomButton {
                id: loginButton
                enabled: !isPageBusy
                text: qsTr("LOG IN")
                onClicked: {
                    if (!login.text.length)
                        window.alert(qsTr("Error!"), qsTr("Please! Enter your login!"), null, null)
                    else if (!password.text.length)
                        window.alert(qsTr("Error!"), qsTr("Please! Enter your password!"), null, null)
                    else
                        requestHttp.post("/login/", JSON.stringify({"login":login.text,"password":password.text}))
                }
            }

            CustomButton {
                id: lostPasswordButton
                usesAlternativeColor: true
                enabled: !isPageBusy
                text: qsTr("Retrieve password")
                onClicked: pageStack.push(pluginAbsPath + "LostPassword.qml")
            }
        }
    }
}
