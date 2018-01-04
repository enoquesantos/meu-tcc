import QtQuick 2.8
import QtQuick.Controls 2.2
import QtQuick.Layouts 1.3

Rectangle {
    id: rootItem
    objectName: "ActionMessage.qml"
    anchors.centerIn: parent; radius: width
    color: "transparent"
    width: visible ? (parent.width * 0.70) : 0; height: width
    transitions: Transition {
        NumberAnimation { property: "opacity"; duration: 250 }
    }
    states: [
        State {
            when: stateVisible
            PropertyChanges { target: rootItem; opacity: 1.0 }
        },
        State {
            when: !stateVisible
            PropertyChanges { target: rootItem; opacity: 0.0 }
        }
    ]

    property alias iconSize: actionMessageIcon.size
    property alias iconName: actionMessageIcon.name
    property alias textColor: actionMessage.color
    property alias messageText: actionMessage.text

    signal clicked()
    signal pressAndHold()

    RoundButton {
        flat: true
        anchors.fill: parent; radius: parent.radius
        onClicked: rootItem.clicked()
        onPressAndHold: rootItem.pressAndHold()
    }

    ColumnLayout {
        spacing: 20
        anchors.centerIn: parent

        AwesomeIcon {
            id: actionMessageIcon
            size: 50; color: actionMessage.color; name: "warning"
            clickEnabled: false
            anchors {
                horizontalCenter: parent.horizontalCenter
                verticalCenter: actionMessage.text ? undefined : parent.verticalCenter
            }
        }

        Text {
            id: actionMessage
            color: Config.theme.textColorPrimary; opacity: 0.8
            renderType: Text.NativeRendering; text: qsTr("None item available!")
            anchors.horizontalCenter: parent.horizontalCenter
            horizontalAlignment: Text.AlignHCenter
            font { weight: Font.DemiBold; pointSize: Config.fontSize.normal }
        }
    }
}
