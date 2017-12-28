import QtQuick 2.9
import QtQuick.Controls 2.2

Popup {
    id: _dialog
    objectName: "AndroidDialog.qml"
    x: Math.round((window.width - width) / 2)
    y: Math.round(window.height / 3.6)
    width: Math.round(Math.min(window.width, window.height) / 3 * 2.5)
    height: (window.height/4) + _message.implicitHeight
    modal: true; focus: true    
    onClosed: {
        showCancelBtn = true
        showConfirmBtn = true
    }

    property bool showCancelBtn: true
    property bool showConfirmBtn: true
    property alias title: _title.text
    property alias text: _message.text

    signal accepted()
    signal rejected()

    Text {
        id: _title
        color: "#444"
        width: parent.width
        font { pointSize: 18; bold: true }
        wrapMode: Label.WordWrap
        horizontalAlignment: Label.AlignLeft
        verticalAlignment: Label.AlignTop
    }

    Text {
        id: _message
        color: "#444"
        width: parent.width
        font.pointSize: _title.font.pointSize - 2
        wrapMode: Label.WordWrap
        horizontalAlignment: Label.AlignLeft
        verticalAlignment: Label.AlignVCenter
        anchors.centerIn: parent
    }

    Row {
        spacing: 10
        anchors { bottom: parent.bottom; bottomMargin: -5; right: parent.right; rightMargin: 2 }

        Button {
            visible: showCancelBtn
            text: qsTr("CANCEL")
            font.bold: true; flat: true; highlighted: true
            onClicked: {
                rejected()
                _dialog.close()
            }
        }

        Button {
            visible: showConfirmBtn
            text: qsTr("OK")
            flat: true; font.bold: true; highlighted: true
            onClicked: {
                accepted()
                _dialog.close()
            }
        }
    }
}
