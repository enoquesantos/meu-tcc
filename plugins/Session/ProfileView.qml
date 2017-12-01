import QtQuick 2.8
import QtQuick.Layouts 1.3
import QtQuick.Controls 2.1

import "qrc:/qml/"
import "qrc:/qml/Awesome/"

BasePage {
    id: page
    title: qsTr("My profile")
    objectName: "ProfileView.qml"
    hasListView: false
    hasNetworkRequest: false

    Flickable {
        id: flickable
        anchors.fill: parent
        contentHeight: Math.max(column.implicitHeight + 50, height)

        ColumnLayout {
            id: column
            spacing: 0
            width: page.width
            anchors { top: parent.top; horizontalCenter: parent.horizontalCenter }

            Rectangle {
                width: parent.width; height: 180; color: Config.theme.colorPrimary
                anchors { top: parent.top; topMargin: 0; horizontalCenter: parent.horizontalCenter }

                Icon {
                    width: 32; height: width
                    name: "pencil"; size: 22
                    color: Config.theme.colorAccent
                    onClicked: pageStack.push(pluginAbsPath + "ProfileEdit.qml", {"pluginAbsPath":pluginAbsPath})
                    anchors { top: parent.top; topMargin: 35; right: parent.right; rightMargin: 20 }
                }

                RoundedImage {
                    id: userImageProfile
                    width: 100; height: width
                    borderColor: Config.theme.colorAccent
                    imgSource: user.profile.image_path || "qrc:/default_user_image.svg"
                    anchors.centerIn: parent
                }
            }

            ListItem {
                showSeparator: true
                primaryLabelText: qsTr("Name")
                secondaryLabelText: user.profile.name
                primaryIconName: "user"
            }

            ListItem {
                showSeparator: true
                primaryLabelText: qsTr("Email")
                secondaryLabelText: user.profile.email
                primaryIconName: "envelope"
            }
        }
    }
}
