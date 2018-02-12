import QtQuick 2.8

// this example, add a new option to menu list
Timer {
    running: true; interval: 5000
    onTriggered: {
        var page = {
            absPath: Config.plugins.examples + "Page1.qml",
            title: "New page",
            awesomeIcon: "gear",
            roles: ["customer"],
            order: 0,
            showInDrawer: true,
            showInTabBar: true
        }
        App.eventNotify(Config.events.appendOptionPage, page)
    }
}
