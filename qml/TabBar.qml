import QtQuick 2.8
import QtQuick.Controls 2.1

TabBar {
    z: 2
    visible: !Config.hasLogin || (Config.hasLogin && user.isLoggedIn)
    currentIndex: swipeView.currentIndex

    // tooltip is visible when user press and hold in some of TabBar buttons
    // and the text displayed in tooltip is the current pressed button text.
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
