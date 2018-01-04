import QtQuick 2.8
import QtQuick.Controls 2.1
import QtGraphicalEffects 1.0

CustomButton {
    id: button
    objectName: "FloatingButton.qml"
    z: 10
    borderColor: backgroundColor
    radius: 200; width: 50; height: width
    maximumWidth: width; maximumHeigth: width
    contentItem: AwesomeIcon {
        id: contentIcon; clickEnabled: false; size: 18
        name: "plus"; anchors.centerIn: parent; color: Config.theme.colorAccent
    }
    anchors {
        horizontalCenter: undefined
        bottom: parent.bottom
        bottomMargin: 16
        right: parent.right
        rightMargin: 16
    }
    layer {
        enabled: true
        effect: DropShadow {
            samples: 17; radius: 12
            color: "#66000000"; spread: 0
            verticalOffset: 1; horizontalOffset: 0
        }
    }

    property alias iconName: contentIcon.name
    property alias iconColor: contentIcon.color
}
