import QtQuick 2.8
import QtQuick.Controls 2.1

import "Awesome/" as Awesome

TabButton {
    id: control
    contentItem: Text {
        visible: Config.showTabButtonText
        text: control.text
        font.pointSize: Config.fontSize.small
        color: icon.color
        horizontalAlignment: Text.AlignHCenter
        anchors { bottom: parent.bottom; bottomMargin: 2 }
        elide: Text.ElideRight

        Behavior on color {
            ColorAnimation { duration: 100 }
        }
    }
    background: Rectangle {
        id: tabButtonRectangle
        width: control.width; height: control.height
        color: control.checked ? Config.theme.colorAccent : Config.theme.colorPrimary

        Behavior on color {
            ColorAnimation { duration: 200 }
        }
    }
    onPressAndHold: {
        window.footer.tooltipX = control.x + ((control.width - control.contentItem.implicitWidth) * 0.35)
        window.footer.tooltipY = control.y - control.height
        window.footer.tooltipText = control.text
    }

    // define the awesome icon
    Awesome.Icon {
        id: icon
        clickEnabled: false
        size: Config.showTabButtonText ? 12 : 20; opacity: control.down ? 0.7 : 1
        color: control.checked ? Config.theme.colorPrimary : Config.theme.colorAccent
        anchors {
            centerIn: Config.showTabButtonText ? undefined : parent
            top: Config.showTabButtonText ? parent.top : undefined
            topMargin: Config.showTabButtonText ? 5 : 0
            horizontalCenter: Config.showTabButtonText ? parent.horizontalCenter : undefined
        }
    }

    Binding {
        target: icon
        property: "color"
        value: Config.theme.colorPrimary
        when: control.checked
    }

    Binding {
        target: icon
        property: "color"
        value: Config.theme.colorAccent
        when: !control.checked
    }

    property alias iconName: icon.name
}
