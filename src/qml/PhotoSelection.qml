import QtQuick 2.8
import QtQuick.Controls 2.1
import QtQuick.Layouts 1.3
import QtGraphicalEffects 1.0

Popup {
    id: _dialog
    objectName: "PhotoSelection.qml"
    x: Math.round((window.width - width) / 2)
    y: Math.round((window.height - height) / 2.5)
    width: Math.round(Math.min(window.width, window.height) / 3 * 2.5)
    height: (window.height/4) + content.implicitHeight*0.70
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

    property string optionChosed
    onOptionChosedChanged: {
        if (optionChosed) {
            parent.forceActiveFocus()
            _dialog.close()
            notify.running = true
        }
    }

    signal removeCurrentPhoto()

    Timer {
        id: notify
        interval: 100
        onTriggered: {
            if (optionChosed == "openCamera")
                pageStack.push("CameraCapture.qml")
            else if (optionChosed == "openGallery")
                FileDialog.open("*/*")
            else if (optionChosed == "removeCurrentPhoto")
                removeCurrentPhoto() // send signal to UserProfile remove the user profile photo
            optionChosed = ""
        }
    }

    Label {
        id: _title
        width: parent.width
        font { pointSize: 20; bold: true }
        wrapMode: Label.WordWrap
        horizontalAlignment: Label.AlignLeft
        verticalAlignment: Label.AlignTop
        text: qsTr("Choose a option")
    }

    Column {
        id: content
        spacing: 0
        width: parent.width
        anchors { top: _title.bottom; topMargin: 30; horizontalCenter: parent.horizontalCenter }

        ListItem {
            primaryIconName: "camera"
            primaryLabelText: qsTr("Open the camera")
            onClicked: optionChosed = "openCamera"
        }

        ListItem {
            primaryIconName: "photo"
            primaryLabelText: qsTr("Choose from gallery")
            onClicked: optionChosed = "openGallery"
        }

        ListItem {
            primaryIconName: "trash"
            primaryLabelText: qsTr("Remove current image")
            onClicked: optionChosed = "removeCurrentPhoto"
        }
    }
}
