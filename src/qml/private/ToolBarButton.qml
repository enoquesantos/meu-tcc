import QtQuick 2.8
import QtQuick.Controls 2.1

import "qrc:/publicComponentes/"

ToolButton {
    id: toolButton
    objectName: "ToolBarButton.qml"
    onClicked: if (actionName) App.eventNotify(actionName, 0)
    width: visible ? toolButton.width : 0; height: parent ? parent.height : 0
    contentItem: AwesomeIcon {
        id: toolButtonIcon
        clickEnabled: false; z: -1; size: 26
        anchors {
            horizontalCenter: parent.horizontalCenter
            verticalCenter: parent.verticalCenter
        }
    }

    property string actionName
    property alias iconName: toolButtonIcon.name
    property alias iconColor: toolButtonIcon.color
}
