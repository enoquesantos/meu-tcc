import QtQuick 2.8

// implementation of Google Material Toast.
// https://material.google.com/components/snackbars-toasts.html

Column {
    id: root
    objectName: "Toast.qml"
    z: Infinity; spacing: 8
    anchors {
        bottom: atCenter ? undefined : parent.bottom
        bottomMargin: atCenter ? undefined : 55
        centerIn: atCenter ? parent : undefined
        horizontalCenter: atCenter ? undefined : parent.horizontalCenter
    }

    property bool atCenter: false

    function show(text, putInCenter, duration) {
        if (putInCenter)
            atCenter = putInCenter
        var toast = toastComponent.createObject(root)
        toast.selfDestroying = true
        toast.show(text, duration)
    }

    Component {
        id: toastComponent

        Rectangle {
            id: root
            opacity: 0
            radius: margin*2; color: "#323232"
            border { color: "transparent"; width: 0 }
            width: childrenRect.width + (2 * margin)
            height: childrenRect.height + (2 * margin)
            anchors.horizontalCenter: parent.horizontalCenter

            property real margin: 12
            property bool selfDestroying: false // Whether this Toast will selfdestroy when it is finished
            property real time: defaultTime
            readonly property real defaultTime: 3000
            readonly property real fadeTime: 350

            /**
             * @brief Shows this Toast
             *
             * @param {string} text Text to show
             * @param {real} duration Duration to show in milliseconds, defaults to 3000
             */
            function show(text, duration) {
                theText.text = text
                if (typeof duration !== "undefined")
                    time = duration >= (2*fadeTime) ? duration : 2*fadeTime
                else
                    time = defaultTime
                anim.start()
            }

            Text {
                id: theText
                text: ""; color: "#fff"
                x: margin; y: margin
                font.pointSize: 14
                horizontalAlignment: Text.AlignHCenter
            }

            SequentialAnimation on opacity {
                id: anim
                running: false
                onRunningChanged: if (!running && selfDestroying) root.destroy()

                NumberAnimation {
                    to: 0.9
                    duration: fadeTime
                }
                PauseAnimation {
                    duration: time - 2*fadeTime
                }
                NumberAnimation {
                    to: 0
                    duration: 2*fadeTime
                }
            }
        }
    }
}
