import QtQuick 2.8
import QtQuick.Controls 2.1

Button {
    id: __customButton
    objectName: "CustomButton.qml"
    implicitWidth: 180; implicitHeight: 38
    z: 1; radius: 25; opacity: enabled ? 1.0 : 0.7
    anchors.horizontalCenter: parent.horizontalCenter
    background: Rectangle {
        id: __backgroundButton
        opacity: __customButton.down ? 0.8 : 1.0
        border { color: borderColor; width: 1 }
        color: usesAlternativeColor ? Config.theme.colorAccent : Config.theme.colorPrimary
        implicitWidth: __customButton.implicitWidth; implicitHeight: __customButton.implicitHeight

        Behavior on opacity {
            NumberAnimation { duration: 450 }
        }
    }
    contentItem: Text {
        id: __customButtonText
        font.pointSize: Config.fontSize.normal
        elide: Text.ElideRight; font.bold: true; color: usesAlternativeColor ? Config.theme.colorPrimary : Config.theme.colorAccent
        text: __customButton.text.toUpperCase(); opacity: __backgroundButton.opacity
        horizontalAlignment: Text.AlignHCenter; verticalAlignment: Text.AlignVCenter
    }

    property bool usesAlternativeColor: false
    property alias radius: __backgroundButton.radius
    property alias maximumWidth: __backgroundButton.width
    property alias maximumHeigth: __backgroundButton.height

    property alias textColor: __customButtonText.color
    property alias backgroundColor: __backgroundButton.color
    property color borderColor: usesAlternativeColor ? Config.theme.colorControlHighlight : Config.theme.colorControl
}
