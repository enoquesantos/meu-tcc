import QtQuick 2.8
import QtQuick.Controls 2.1
import QtQuick.Controls.Material 2.1
import QtQuick.Controls.Material.impl 2.1

import "Map.js" as Awesome

Item {
    id: widget
    antialiasing: true
    opacity: enabled ? 1.0 : 0.7
    width: text.width; height: text.height

    property string name
    property int size: 16
    property bool shadow: false
    property alias color: text.color
    property alias weight: text.font.weight
    readonly property var icons: Awesome.map
    property bool valid: text.implicitWidth > 0
    property bool rotate: widget.name.match(/.*-rotate/) !== null
    property bool clickEnabled: true
    property alias control: _control
    property int cursorType: Qt.PointingHandCursor

    signal clicked()
    signal pressAndHold()
    signal entered()
    signal exited()

    FontLoader { id: fontAwesome; source: Qt.resolvedUrl("Font.otf") }

    Text {
        id: text
        antialiasing: true
        anchors.centerIn: parent
        styleColor: Qt.rgba(0,0,0,0.5)
        color: Config.theme.textColorPrimary; style: shadow ? Text.Raised : Text.Normal
        text: widget.icons.hasOwnProperty(name) ? widget.icons[name] : ""
        font { pointSize: size < 1 ? 16 : size; weight: Font.Light; family: fontAwesome.name }

        property string name: widget.name.match(/.*-rotate/) !== null ? widget.name.substring(0, widget.name.length - 7) : widget.name

        Behavior on color {
            ColorAnimation { duration: 200 }
        }

        NumberAnimation on rotation {
            from: 0; to: 360; running: widget.rotate
            duration: 1100; loops: Animation.Infinite
        }
    }

    MouseArea {
        id: _control
        z: clickEnabled ? parent.z + 1 : -1
        anchors.fill: clickEnabled ? parent : undefined
        onClicked: if (clickEnabled) widget.clicked()
        onPressAndHold: if (clickEnabled) widget.pressAndHold()
    }

    Ripple {
        id: ripple
        z: 0
        anchor: widget
        x: (parent.width - width) / 2
        y: (parent.height - height) / 2
        width: widget.width * 1.4; height: width
        pressed: clickEnabled && _control.pressed
        active: clickEnabled && _control.pressed
        color: clickEnabled && _control.pressed ? Material.highlightedRippleColor : Material.rippleColor
    }
}
