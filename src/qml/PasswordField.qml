import QtQuick 2.9
import QtQuick.Controls 2.2

TextField {
    id: textField
    z: 0; antialiasing: true; width: parent.width
    color: Config.theme.textColorPrimary
    echoMode: TextInput.Password
    selectByMouse: true; cursorVisible: focus
    anchors.horizontalCenter: parent.horizontalCenter
    renderType: Text.NativeRendering
    onAccepted: if (nextFocusItem) nextFocusItem.focus = true
    onEditingFinished: text = text.trim()

    property Item nextFocusItem
    property bool iconClickEnabled: !textField.readOnly

    AwesomeIcon {
        id: icon
        visible: Qt.platform.os !== "ios"
        width: 25; height: width; y: 1
        clickEnabled: iconClickEnabled
        z: 1; size: 25; color: textField.color
        name: textField.echoMode === TextInput.Password ? "eye" : "eye_slash"
        anchors { right: parent ? parent.right : undefined; rightMargin: 0 }
        onClicked: textField.echoMode = textField.echoMode === TextInput.Password ? TextInput.Normal : TextInput.Password
    }
}
