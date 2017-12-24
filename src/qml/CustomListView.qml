import QtQuick 2.8
import QtQuick.Controls 2.1

ListView {
    objectName: "CustomListView.qml"
    model: _listModel
    cacheBuffer: 1600
    width: window.width
    anchors.fill: parent ? parent : undefined
    add: Transition {
        NumberAnimation { property: "opacity"; from: 0; to: 1.0; duration: 150 }
    }
    displaced: Transition {
        NumberAnimation { properties: "x,y"; duration: 150; easing.type: Easing.InOutQuint }
    }
    ScrollIndicator.vertical: ScrollIndicator { }

    property ListModel _listModel: ListModel { }
}
