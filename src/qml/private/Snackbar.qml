import QtQuick 2.8
import QtQuick.Layouts 1.3
import QtQuick.Controls 2.1

// implementation of Google Material Snackbar.
// https://material.google.com/components/snackbars-toasts.html

Item {
    id: snackbar
    z: 2
    objectName: "Snackbar.qml"
    width: parent.width; height: 48
    anchors { bottom: parent.bottom; bottomMargin: initialMargin }

    property int initialMargin: -48
    property int visibleMargin: 0

    NumberAnimation {
        id: animateShowOpacity
        target: snackbar
        properties: "anchors.bottomMargin"
        from: initialMargin; to: visibleMargin; duration: 150
        onStopped: {
            if (closePending)
                closed()
            isOpened = true
            openPending = false
        }
    }

    NumberAnimation {
        id: animateHideOpacity
        target: snackbar
        properties: "anchors.bottomMargin"
        from: visibleMargin; to: initialMargin; duration: 150
        onStopped: {
            message.text = ""
            if (openPending)
                reopen.start()
            isOpened = false
            closePending = false
            if (closeCallback.length > 0 && typeof closeCallback[0] === "function") {
                closeCallback[0]()
                closeCallback.splice(0, 1)
            }
        }
    }

    signal opened()
    signal closed()

    property bool isOpened: false
    property bool openPending: false
    property bool closePending: false
    property bool isLongDuration: false
    property string messageTemp: ""
    property alias actionText: action.text
    property alias actionTextColor: action.color

    property var closeCallback: []
    property var actionCallback

    function close() {
        closed()
        actionText = ""
    }

    function show(s, callback) {
        if (callback)
            closeCallback.push(callback)
        messageTemp = s
        if (isOpened) {
            openPending = true
            close()
        } else {
            opened()
        }
    }

    onClosed: {
        if (!animateHideOpacity.running)
            animateHideOpacity.start()
    }

    onOpened: {
        if (!animateShowOpacity.running) {
            message.text = messageTemp
            if (countdownToClose.running)
                countdownToClose.restart()
            else
                countdownToClose.start()
            if (animateShowOpacity.running)
                animateShowOpacity.stop()
            animateShowOpacity.start()
        }
    }

    Timer {
        id: countdownToClose
        repeat: false; interval: isLongDuration ? 7000 : 3000
        onTriggered: closed()
    }

    Timer {
        id: reopen
        repeat: false; interval: 500
        onTriggered: opened()
    }

    Rectangle {
        id: rectSnack
        anchors.fill: parent
        color: "#323232"

        RowLayout {
            width: parent.width; height: parent.height
            anchors { left: parent.left; leftMargin: 15; right: parent.right; verticalCenter: parent.verticalCenter }

            Label {
                id: message
                antialiasing: true
                width: actionCallback ? snackbar.width * 0.65 : snackbar.width * 0.90
                elide: Label.ElideRight
                wrapMode: Label.WrapAnywhere
                renderType: Label.NativeRendering
                color: "#fff"; font.pointSize: 14
                verticalAlignment: Label.AlignVCenter
                horizontalAlignment: Label.AlignLeft
                onTextChanged: {
                    while (snackbar.height < message.text.contentHeight)
                        snackbar.height *= 1.2
                }
            }

            Rectangle {
                id: actionRec
                color: "transparent"
                anchors.right: parent.right
                height: parent.height; width: parent.width * 0.28

                Text {
                    id: action
                    antialiasing: true
                    anchors.centerIn: parent
                    elide: Text.ElideRight
                    wrapMode: Text.WrapAnywhere
                    renderType: Text.NativeRendering
                    color: "#e7f740"; font.pointSize: 14
                }

                MouseArea {
                    anchors.fill: parent
                    onEntered: actionRec.color = Qt.rgba(0,0,0,0.2)
                    onExited: actionRec.color = "transparent"
                    onClicked: {
                        countdownToClose.stop()
                        animateHideOpacity.restart()
                        if (typeof actionCallback === "function")
                            actionCallback()
                    }
                }
            }
        }
    }
}
