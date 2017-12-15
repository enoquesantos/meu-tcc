import QtQuick 2.8

Item {
    objectName: "ListenersLoader.qml"
    visible: false

    Component.onCompleted: {
        var component = ({}), listeners = App.readSetting("listeners", App.SettingTypeJsonArray)
        for (var i = 0; i < listeners.length; i++) {
            component = Qt.createComponent(listeners[i])
            if (component.status === Component.Ready)
                component.createObject(window, {"parent":window})
            else
                console.error("Error on try load a Listneter Component: ", component.errorString())
        }
        component = null
        listeners = null
    }
}
