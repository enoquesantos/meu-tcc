import QtQuick 2.8
import QtQuick.Controls 2.1

TextField {
    id: textField
    objectName: "PasswordField.qml"
    z: 0; antialiasing: true
    color: Config.theme.textColorPrimary
    width: parent.width
    selectByMouse: true
    cursorVisible: focus
    anchors.horizontalCenter: parent.horizontalCenter
    renderType: Text.NativeRendering
    background: Rectangle {
        color: textField.color
        y: (textField.height-height) - (textField.bottomPadding / 1.3)
        width: textField.width; height: textField.activeFocus ? 2 : 1
        border { width: 1; color: textField.color }
    }
    onAccepted: if (nextFocusItem) nextFocusItem.focus = true
    onEditingFinished: text = text.trim()

    property Item nextFocusItem
    property bool iconClickEnabled: !textField.readOnly
    property string iconName

    Loader {
        id: loader
        active: Qt.platform.os !== "ios" && parent.echoMode === TextInput.Password
        asynchronous: true
        onLoaded: item.parent = textField
        sourceComponent: AwesomeIcon {
            id: icon
            width: 25; height: width; y: 1
            clickEnabled: iconClickEnabled
            z: 1; size: 25; color: textField.color
            name: iconName ? iconName : textField.echoMode === TextInput.Password ? "eye" : "eye_slash"
            anchors { right: parent ? parent.right : undefined; rightMargin: 0 }
            onClicked: {
                textField.echoMode = textField.echoMode === TextInput.Password ? TextInput.Normal : TextInput.Password
                loader.active = true
                icon.visible = true
                z = 1
            }
        }
    }
}
