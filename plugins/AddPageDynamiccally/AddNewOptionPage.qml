import QtQuick 2.9

// this example, add a new option to menu list
Timer {
    running: true; interval: 5000
    onTriggered: {
        var pageSettings = {
            absPath: Config.plugins.addpagedynamiccally + "Page1.qml",
            title: "New page",
            awesomeIcon: "gear",
            roles: ["customer", "student", "teacher", "coordinator"],
            order: 0,
            showInDrawer: true,
            showInTabBar: true
        }
        Subject.notify(Config.events.appendOptionPage, pageSettings)
    }
}
