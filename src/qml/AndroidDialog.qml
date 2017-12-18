import QtQuick 2.8
import QtQuick.Controls 2.1
import QtQuick.Controls.Material 2.1
import QtGraphicalEffects 1.0

Popup {
    id: _dialog
    x: Math.round((window.width - width) / 2)
    y: Math.round(window.height / 3.6)
    width: Math.round(Math.min(window.width, window.height) / 3 * 2.5)
    height: (window.height/4) + _label.implicitHeight
    modal: true; focus: true
    background: Rectangle {
        id: rect
        color: "#fff"; radius: 4
        width: _dialog.width; height: _dialog.height

        RectangularGlow {
            z: -1; color: "#444"
            anchors.fill: rect
            cached: true
            glowRadius: 4; spread: 0.0; cornerRadius: 4
        }
    }

    property alias title: _title.text
    property alias text: _label.text

    signal accepted()
    signal rejected()

    Label {
        id: _title
        width: parent.width
        font { pointSize: 20; bold: true }
        wrapMode: Label.WordWrap
        horizontalAlignment: Label.AlignLeft
        verticalAlignment: Label.AlignTop
    }

    Label {
        id: _label
        width: parent.width
        font.pointSize: 15
        wrapMode: Label.WordWrap
        horizontalAlignment: Label.AlignLeft
        verticalAlignment: Label.AlignVCenter
        anchors.centerIn: parent
    }

    Row {
        spacing: 10
        anchors { bottom: parent.bottom; bottomMargin: -5; right: parent.right; rightMargin: 5 }

        Button {
            text: qsTr("CANCEL")
            font.bold: true
            highlighted: false
            Material.elevation: 0
            Material.foreground: Material.BlueGrey
            onClicked: {
                rejected()
                _dialog.close()
            }
        }

        Button {
            text: qsTr("OK")
            font.bold: true
            highlighted: false
            Material.elevation: 0
            Material.foreground: Material.BlueGrey
            onClicked: {
                accepted()
                _dialog.close()
            }
        }
    }
}
