import QtQuick 2.8
import QtQuick.Controls 2.1
import QtGraphicalEffects 1.0

TabBar {
    id: tabBar
    z: 2
    visible: !Config.hasLogin || (Config.hasLogin && window.userProfile && window.userProfile.isLoggedIn)
    currentIndex: swipeView.currentIndex

    Loader {
        onLoaded: tabBar.background = item
        asynchronous: false; active: true
        sourceComponent: Rectangle {
            color: "#fff"
            width: tabBar.width; height: tabBar.height
            layer {
                enabled: Config.applicationStyle.indexOf("Material") > -1
                effect: DropShadow {
                    samples: 17; radius: 12
                    color: "#4D888888"; spread: 0.1
                    verticalOffset: 0; horizontalOffset: 0
                }
            }
        }
    }

    // to reduce the couplin with anothers components, the Drawer menu listener events
    // from the application. This connection handle the signals:
    //     1: a signal to add new option to list pages in menu
    Connections {
        target: App
        onEventNotify: {
            // signal signature: eventNotify(string eventName, var eventData)
            if (eventName === Config.events.appendOptionPage)
                functions.addNewMenuItem(eventData, true)
        }
    }

    // tooltip can be visible when user press and hold in any of buttons from TabBar
    // and the text displayed in tooltip is the pressed button text.
    ToolTip {
        id: tooltip
        visible: text.length > 0; timeout: 1500
        background: Rectangle { anchors.fill: parent; color: Config.theme.toolTipBackgroundColor; radius: 25; opacity: 0.8 }
        onClosed: text = ""
    }

    property alias tooltipX: tooltip.x
    property alias tooltipY: tooltip.y
    property alias tooltipText: tooltip.text
}
