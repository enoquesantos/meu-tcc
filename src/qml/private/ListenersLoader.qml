import QtQuick 2.8

QtObject {
    objectName: "ListenersLoader.qml"

    Component.onCompleted: {
        var i = 0, component = {}, listeners = App.readSetting("listeners", App.SettingTypeJsonArray)
        for (; i < listeners.length; i++) {
            component = Qt.createComponent(listeners[i])
            if (component.status === Component.Ready)
                component.createObject(window, {"parent":window})
            else
                console.error("Error on try load a Listener Component: ", component.errorString())
        }
        i = 0; component = null; listeners = null
    }
}
