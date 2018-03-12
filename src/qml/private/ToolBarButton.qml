import QtQuick.Controls 2.3

import "qrc:/publicComponentes/" as Components

ToolButton {
    onClicked: if (actionName) Subject.notify(actionName, 0)

    Components.AwesomeIcon {
        id: toolButtonIcon
        size: 21
        clickEnabled: false
        anchors { horizontalCenter: parent.horizontalCenter; verticalCenter: parent.verticalCenter }
    }

    property string actionName
    property alias iconName: toolButtonIcon.name
    property alias iconColor: toolButtonIcon.color
}
