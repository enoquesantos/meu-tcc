import QtQuick 2.8
import QtQuick.Controls 2.1

RadioButton {
    id: control
    indicator: Rectangle {
        implicitWidth: 26
        implicitHeight: 26
        x: control.leftPadding
        y: parent.height / 2 - height / 2
        radius: 13
        border.color: Config.theme.colorPrimary

        Rectangle {
            width: 14
            height: 14
            x: 6
            y: 6
            radius: 7
            color: Config.theme.colorPrimary
            visible: control.checked
        }
    }
    contentItem: Text {
        text: control.text
        font: control.font
        opacity: enabled ? 1.0 : 0.3
        color: Config.theme.colorPrimary
        horizontalAlignment: Text.AlignHCenter
        verticalAlignment: Text.AlignVCenter
        leftPadding: control.indicator.width + control.spacing
    }
}
