import QtQuick 2.8

Item {
    objectName: "Listener1.qml"

    Component.onCompleted: console.log(objectName + " created!")
    Component.onDestruction: console.log(objectName + " onDestruction!")
}
