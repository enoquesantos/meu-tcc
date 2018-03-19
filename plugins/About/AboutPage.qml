import QtQuick 2.9
import QtQuick.Controls 2.2
import QtQuick.Layouts 1.3

import "qrc:/publicComponents/" as Components

Components.BasePage {
    id: page
    title: qsTr("About ") + Config.applicationName
    objectName: "AboutPage.qml"
    absPath: Config.plugins.about + "AboutPage.qml"
    hasListView: false
    hasNetworkRequest: false
    pageBackgroundColor: Config.theme.colorPrimary

    Flickable {
        id: flickable
        anchors.fill: parent
        contentHeight: Math.max(column.implicitHeight + 50, height)

        ColumnLayout {
            id: column
            spacing: 20
            width: page.width
            anchors { top: parent.top; horizontalCenter: parent.horizontalCenter }

            Components.RoundedImage {
                id: logo
                width: 64; height: 64
                imgSource: "qrc:/icon.png"
                anchors.horizontalCenter: parent.horizontalCenter
                onClicked: animationTerminator.running = !animationTerminator.running

                NumberAnimation on rotation {
                    from: 0; to: 360; running: animationTerminator.running
                    duration: 150; loops: Animation.Infinite
                }

                Timer {
                    id: animationTerminator
                    interval: 600; running: true
                    onTriggered: logo.rotation = 0
                }
            }

            Item {
                id: firstLabelItem
                width: parent.width * 0.80; height: applicationName.implicitHeight + 10
                anchors.horizontalCenter: parent.horizontalCenter

                Label {
                    id: applicationName
                    width: parent.width
                    wrapMode: Text.WordWrap
                    text: Config.applicationName
                    color: Config.theme.colorAccent
                    font { weight: Font.ExtraBold; pointSize: Config.fontSize.extraLarge }
                    horizontalAlignment: Label.AlignHCenter
                    anchors.horizontalCenter: parent.horizontalCenter

                    MouseArea {
                        anchors.fill: parent
                        onClicked: Qt.openUrlExternally("www.google.com")
                    }
                }
            }

            Item {
                width: parent.width * 0.90
                height: appDetailedDescription.height
                anchors {
                    horizontalCenter: parent.horizontalCenter
                    top: firstLabelItem.bottom
                    topMargin: 25
                }

                Text {
                    id: appDetailedDescription
                    width: parent.width
                    wrapMode: Text.WordWrap
                    elide: Text.ElideRight
                    color: Config.theme.colorAccent
                    font { weight: Font.DemiBold; pointSize: 13 }
                    horizontalAlignment: Label.AlignJustify
                    anchors.horizontalCenter: parent.horizontalCenter
                    text: Utils.readFile(Qt.resolvedUrl("AppDescription.txt"))
                    textFormat: Text.RichText
                    onLinkActivated: Qt.openUrlExternally(link)
                }
            }
        }
    }
}
