import QtQuick 2.8
import QtQuick.Controls 2.1
import QtGraphicalEffects 1.0

TabBar {
    id: tabBar
    z: 2
    objectName: "TabBar.qml"
    visible: !Config.hasLogin || (Config.hasLogin && userProfile.isLoggedIn)
    currentIndex: swipeView.currentIndex

    Loader {
        onLoaded: tabBar.background = item
        asynchronous: false; active: true
        sourceComponent: Rectangle {
            z: 0
            color: "#fff"; border.width: 0; radius: 0
            width: tabBar.width; height: tabBar.height
            layer {
                enabled: true
                effect: DropShadow {
                    samples: 17; radius: 15
                    color: "#4D000000"; spread: 0.5
                    verticalOffset: 1; horizontalOffset: 0
                }
            }
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
