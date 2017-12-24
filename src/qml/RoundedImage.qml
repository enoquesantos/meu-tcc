import QtQuick 2.8
import QtQuick.Controls 2.0

Rectangle {
    id: roundedImage
    objectName: "RoundedImage.qml"
    antialiasing: true
    color: "transparent"
    visible: img.source

    property alias control: _control
    property alias imgSource: img.source
    property color borderColor: "transparent"

    signal imageReady()
    signal clicked()
    signal pressAndHold()

    MouseArea {
        id: _control
        anchors.fill: parent
        onClicked: roundedImage.clicked()
        onPressAndHold: roundedImage.pressAndHold()
    }

    // this Rectangle is needed to keep the source image's fillMode
    Rectangle {
        id: imageSource
        anchors.fill: parent
        color: parent.color
        visible: false; layer.enabled: true; antialiasing: true
        states: [
            State {
                when: img.progress === 1.0
                PropertyChanges { target: img; opacity: 1.0 }
            },
            State {
                when: img.progress !== 1.0
                PropertyChanges { target: img; opacity: 0.0 }
            }
        ]
        transitions: [
            Transition {
                NumberAnimation {
                    property: "opacity"; duration: 300
                }
            }
        ]

        Image {
            id: img
            visible: img.source
            sourceSize.width: img.width
            sourceSize.height: img.height
            fillMode: Image.PreserveAspectCrop
            clip: true; cache: true; smooth: true
            asynchronous: true; anchors.fill: parent
            onStatusChanged: if (status === Image.Ready) roundedImage.imageReady()
        }
    }

    Timer {
        id: timer
        repeat: false
        interval: 30000
        running: img.status == Image.Loading
        onTriggered: if (img.status !== Image.Ready) img.status = Image.Error
    }

    BusyIndicator {
        id: busy
        z: 1; width: 48; height: 48
        visible: img.status === Image.Loading
        antialiasing: true; anchors.centerIn: parent
    }

    Rectangle {
        antialiasing: true
        color: "#e0ffff"
        layer.enabled: true
        anchors.fill: parent
        radius: parent.width
        layer.samplerName: "maskSource"
        layer.effect: ShaderEffect {
            property var colorSource: imageSource
            fragmentShader: "
                uniform lowp sampler2D colorSource;
                uniform lowp sampler2D maskSource;
                uniform lowp float qt_Opacity;
                varying highp vec2 qt_TexCoord0;
                void main() {
                    gl_FragColor = texture2D(colorSource, qt_TexCoord0) * texture2D(maskSource, qt_TexCoord0).a * qt_Opacity;
                }"
        }
    }

    // only for draw border line
    Rectangle {
        border.width: 1; anchors.fill: parent
        color: "transparent"; radius: parent.width
        border.color: borderColor; antialiasing: true
    }
}
