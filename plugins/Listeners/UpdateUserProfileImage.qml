import QtQuick 2.9
import Observer 1.0

Item {
    id: rootItem
    objectName: "UpdateUserProfileImage.qml"

    Component.onCompleted: console.log(objectName + " created!")
    Component.onDestruction: console.log(objectName + " deleted!")

    Observer {
        id: cameraCaptureImageObserver
        objectName: rootItem.objectName
        event: Config.events.cameraImageSaved
        onUpdated: Subject.notify(Config.events.setUserProperty, {"key":"image_path","value":eventData})
        Component.onCompleted: Subject.attach(cameraCaptureImageObserver, event)
    }
}
