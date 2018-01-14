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
