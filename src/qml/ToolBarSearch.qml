import QtQuick 2.8
import QtQuick.Controls 2.1

Item {
    objectName: "ToolBarSearch.qml"
    width: parent.width * 0.65; height: parent.height

    property alias searchText: __searchInput.text

    TextField {
        id: __searchInput
        visible: parent.visible
        focus: visible
        color: defaultTextColor
        placeholderText: qsTr(" tap to search...")
        selectionColor: Qt.rgba(25,25,25,0.2)
        selectedTextColor: Qt.rgba(25,25,25,0.8)
        anchors.verticalCenter: parent.verticalCenter
        cursorDelegate: Rectangle {
            width: 2; height: __searchInput.height / 2
            color: defaultTextColor

            Timer {
                interval: 600; repeat: true; running:__searchInput.visible
                onTriggered: parent.visible = !parent.visible
            }
        }
        onVisibleChanged: {
            var strTemp = ""
            __searchInput.text = strTemp
            if (__searchInput.visible)
                __searchInput.forceActiveFocus()
        }
        background: Rectangle {
            color: defaultTextColor
            border { width: 1; color: defaultTextColor }
            y: (__searchInput.height-height) - (__searchInput.bottomPadding / 1.3)
            width: __searchInput.width; height: __searchInput.activeFocus ? 2 : 1
        }
    }
}
