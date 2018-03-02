import QtQuick.Controls 2.2

import "qrc:/publicComponentes/" as Components

ToolButton {
    z: 2
    onClicked: if (actionName) App.eventNotify(actionName, 0)

    Components.AwesomeIcon {
        id: toolButtonIcon
        size: 18; z: -1
        clickEnabled: false
        anchors { horizontalCenter: parent.horizontalCenter; verticalCenter: parent.verticalCenter }
    }

    property string actionName
    property alias iconName: toolButtonIcon.name
    property alias iconColor: toolButtonIcon.color
}
