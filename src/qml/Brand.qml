import QtQuick 2.8
import QtQuick.Controls 2.1

Rectangle {
    objectName: "Brand.qml"
    color: "transparent"
    width: parent.width; height: 150
    anchors {
        top: parent.top
        topMargin: 50
        horizontalCenter: parent.horizontalCenter
    }

    property alias imgSource: logo.source

    Image {
        id: logo
        width: 100; height: width
        source: "qrc:/icon.png"
        anchors.centerIn: parent
    }
}
