import QtQuick 2.9
import QtQuick.Controls 2.2

import "qrc:/publicComponentes/" as Components

TabButton {
    id: control
    contentItem: Text {
        id: txt
        visible: Config.showTabButtonText && swipeView.count < 6
        text: control.text
        font { pointSize: Config.fontSize.small; capitalization: Font.AllUppercase }
        color: awesomeIcon.color; opacity: awesomeIcon.opacity
        horizontalAlignment: Text.AlignHCenter
        anchors { bottom: parent.bottom; bottomMargin: 0 }
        width: control.width * 0.90; elide: Text.ElideRight
    }
    onPressAndHold: {
        window.footer.tooltipX = control.x + ((control.width - control.contentItem.implicitWidth) * 0.10)
        window.footer.tooltipY = control.y - control.height
        window.footer.tooltipText = control.text
    }

    Components.AwesomeIcon {
        id: awesomeIcon
        clickEnabled: false
        size: Config.showTabButtonText && txt.visible ? 12 : 20; opacity: control.down ? 0.7 : 1
        color: control.checked ? Config.theme.colorPrimary : Config.theme.colorAccent
        anchors {
            centerIn: Config.showTabButtonText && txt.visible ? undefined : parent
            horizontalCenter: Config.showTabButtonText && txt.visible ? parent.horizontalCenter : undefined
            top: Config.showTabButtonText && txt.visible ? parent.top : undefined
            topMargin: Config.showTabButtonText && txt.visible ? -24 : undefined
        }
    }

    property alias iconName: awesomeIcon.name
}
