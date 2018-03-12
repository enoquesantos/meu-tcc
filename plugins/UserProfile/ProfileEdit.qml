import QtQuick 2.9
import QtQuick.Layouts 1.3
import QtQuick.Controls 2.2

import "qrc:/publicComponentes/" as Components

Components.BasePage {
    id: page
    title: qsTr("Edit my profile")
    hasListView: false; hasNetworkRequest: false
    toolBarState: "goBack"
    absPath: Config.plugins.userprofile + "ProfileEdit.qml"
    toolBarButtons: [ // add 'save' icon in window.toolbar
        {
            "iconName": "save",
            "callback": function() {
                var _regex = /^(([^<>()\[\]\\.,;:\s@"]+(\.[^<>()\[\]\\.,;:\s@"]+)*)|(".+"))@((\[[0-9]{1,3}\.[0-9]{1,3}\.[0-9]{1,3}\.[0-9]{1,3}])|(([a-zA-Z\-0-9]+\.)+[a-zA-Z]{2,}))$/
                email.text = email.text.toString().trim()
                if (!email.text) {
                    functions.alert(qsTr("Error!"), qsTr("The email field is blank!"))
                    return
                } else if (!_regex.test(email.text)) {
                    functions.alert(qsTr("Error!"), qsTr("The email is not valid!"))
                    return
                } else if (password1.text !== password2.text) {
                    functions.alert(qsTr("Error!"), qsTr("The passwords does not match!"))
                    return
                } else if (password1.text && password1.text == password2.text) {
                    page.focus = true

                    var args = window.userProfile.profile
                    args["email"] = email.text
                    args["password"] = password1.text

                    toast.show(qsTr("Updating profile..."), true)
                    Subject.notify(Config.events.updateUserProfile, args)

                    password1.text = password2.text = ""
                }
            }
        }
    ]

    Connections {
        target: FileDialog
        onAccepted: if (fileUrl) Subject.notify(Config.events.setUserProperty, {"key":"image_path","value":fileUrl})
    }

    Components.PhotoSelection {
        id: photoSelection
    }

    Flickable {
        id: flickable
        anchors.fill: parent
        contentHeight: Math.max(content.implicitHeight, height * 1.5)

        Behavior on contentY {
            NumberAnimation {
                duration: 350
            }
        }

        ColumnLayout {
            id: content
            spacing: 30
            width: page.width * 0.90
            anchors { top: parent.top; topMargin: 15; horizontalCenter: parent.horizontalCenter }

            Rectangle {
                width: parent.width; height: 150; color: "transparent"
                anchors { top: parent.top; topMargin: 0; horizontalCenter: parent.horizontalCenter }

                Components.RoundedImage {
                    width: 90; height: width
                    borderColor: Config.theme.colorPrimary
                    imgSource: userProfile.profile.image_path
                    anchors.horizontalCenter: parent.horizontalCenter
                    anchors { bottom: parent.bottom; bottomMargin: 15 }
                    onClicked: photoSelection.open()
                }
            }

            Rectangle {
                width: parent.width * 0.90; height: 145
                color: "transparent"
                anchors.horizontalCenter: parent.horizontalCenter

                Label {
                    width: parent.width; height: parent.height
                    elide: Label.ElideRight
                    wrapMode: "WordWrap"
                    color: Config.theme.textColorPrimary
                    font.pointSize: Config.fontSize.large
                    horizontalAlignment: Text.AlignJustify
                    anchors.horizontalCenter: parent.horizontalCenter
                    text: qsTr("To update your password, enter the new password's in the fields below. Otherwise, leave the fields empty. We recommend that you create a strong password with characters, letters and numbers!")
                }
            }

            TextField {
                id: email
                color: Config.theme.textColorPrimary
                selectByMouse: true; cursorVisible: focus
                Layout.fillWidth: true
                renderType: Text.NativeRendering
                anchors.horizontalCenter: parent.horizontalCenter
                text: userProfile.profile.email
                font.capitalization: Font.AllLowercase
                inputMethodHints: Qt.ImhEmailCharactersOnly
                placeholderText: qsTr("youremail@example.com")
                onAccepted: password1.focus = true
                onEditingFinished: text = text.trim()
                onFocusChanged: if (focus) flickable.contentY = email.y
            }

            Components.PasswordField {
                id: password1
                nextFocusItem: password2
                echoMode: TextInput.Password
                Layout.fillWidth: true
                inputMethodHints: Qt.ImhNoPredictiveText
                placeholderText: qsTr("Set the new password")
                onFocusChanged: if (focus) flickable.contentY = email.y
            }

            Components.PasswordField {
                id: password2
                echoMode: TextInput.Password
                Layout.fillWidth: true
                inputMethodHints: Qt.ImhNoPredictiveText
                placeholderText: qsTr("Confirm the new password")
                onAccepted: page.focus = true
                onFocusChanged: if (focus) flickable.contentY = email.y
            }
        }
    }
}
