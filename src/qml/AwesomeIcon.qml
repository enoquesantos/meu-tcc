import QtQuick 2.9
import QtQuick.Controls 2.2
import Qt.project.AwesomeIconFontLoader 1.0

Item {
    id: widget
    width: parent.width; height: width

    property string name
    property int size: 16
    property bool shadow: false
    property alias color: text.color
    property alias weight: text.font.weight
    property bool rotate: widget.name.match(/.*-rotate/) !== null
    property bool clickEnabled: true

    signal clicked
    signal pressAndHold

    MouseArea {
        id: mouseArea
        enabled: clickEnabled
        anchors.fill: parent
        onClicked: widget.clicked()
        onPressAndHold: widget.pressAndHold()
    }

    Item {
        z: 1
        anchors.fill: parent
        width: parent.weight; height: width

        Text {
            id: text
            antialiasing: true
            anchors.centerIn: parent
            verticalAlignment: Text.AlignVCenter
            horizontalAlignment: Text.AlignHCenter
            styleColor: Qt.rgba(0,0,0,0.5)
            color: Config.theme.textColorPrimary; style: shadow ? Text.Raised : Text.Normal
            text: name in IconFontLoaderSingleton.iconsMap ? IconFontLoaderSingleton.iconsMap[name] : ""
            font { pointSize: size < 1 ? 16 : size; weight: Font.Light; family: IconFontLoaderSingleton.font }

            property string name: widget.name.match(/.*-rotate/) !== null ? widget.name.substring(0, widget.name.length - 7) : widget.name

            Behavior on color {
                ColorAnimation { duration: 200 }
            }

            NumberAnimation on rotation {
                from: 0; to: 360; running: widget.rotate
                duration: 1100; loops: Animation.Infinite
            }
        }
    }
}

