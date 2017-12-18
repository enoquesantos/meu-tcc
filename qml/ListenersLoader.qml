import QtQuick 2.8

QtObject {
    objectName: "ListenersLoader.qml"

    Component.onCompleted: {
        var component = ({}), listeners = App.readSetting("listeners", App.SettingTypeJsonArray)
        for (var i = 0; i < listeners.length; i++) {
            component = Qt.createComponent(listeners[i])
            if (component.status === Component.Ready)
                component.createObject(window, {"parent":window})
            else
                console.error("Error on try load a Listener Component: ", component.errorString())
        }
        component = null
        listeners = null
    }
}
