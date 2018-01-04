import QtQuick 2.8
import QtQuick.Controls 2.1

ToolButton {
    id: toolButton
    objectName: "ToolBarButton.qml"
    onClicked: App.eventNotify(action, 0); z: 100
    width: visible ? toolButton.width : 0; height: parent.height
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
