import QtQuick 2.8

QtObject {
    Component.onCompleted: {
        var component = {}, listeners = App.readSetting("listeners", App.SettingTypeJsonArray)
        while (listeners.length) {
            component = Qt.createComponent(listeners.pop())
            if (component.status === Component.Ready)
                component.createObject(window)
            else
                console.error("Listener Component Error: ", component.errorString())
        }
        component = null; listeners = null
    }
}
