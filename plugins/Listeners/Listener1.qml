import QtQuick 2.9

Item {
    objectName: "Listener1.qml"

    Component.onCompleted: console.log(objectName + " created!")
    Component.onDestruction: console.log(objectName + " onDestruction!")
}
