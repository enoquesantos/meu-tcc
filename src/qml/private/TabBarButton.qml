import QtQuick 2.8
import QtQuick.Controls 2.1

import "../" as Componentes

TabButton {
    id: control
    objectName: "TabBarButton.qml"
    contentItem: Text {
        visible: Config.showTabButtonText
        text: control.text
        font.pointSize: Config.fontSize.small
        color: icon.color; opacity: icon.opacity
        horizontalAlignment: Text.AlignHCenter
        anchors { bottom: parent.bottom; bottomMargin: 2 }
        width: control.width * 0.90; elide: Text.ElideRight

        Behavior on color {
            ColorAnimation { duration: 150 }
        }
    }
    background: Rectangle {
        id: tabButtonRectangle
        width: control.width; height: control.height + 20
        color: control.checked ? Config.theme.colorAccent : Config.theme.colorPrimary

        Behavior on color {
            ColorAnimation { duration: 150 }
        }
    }
    onPressAndHold: {
        window.footer.tooltipX = control.x + ((control.width - control.contentItem.implicitWidth) * 0.15)
        window.footer.tooltipY = control.y - control.height
        window.footer.tooltipText = control.text
    }

    Componentes.AwesomeIcon {
        id: icon
        clickEnabled: false; height: parent.height*2
        size: Config.showTabButtonText ? 12 : 20; opacity: control.down ? 0.7 : 1
        color: control.checked ? Config.theme.colorPrimary : Config.theme.colorAccent
        anchors {
            centerIn: Config.showTabButtonText ? undefined : parent
            horizontalCenter: Config.showTabButtonText ? parent.horizontalCenter : undefined
            top: Config.showTabButtonText ? parent.top : undefined
            topMargin: Config.showTabButtonText ? (Qt.platform.os === "linux" || Qt.platform.os === "osx" ? -27 : -22) : undefined
        }
    }

    property alias iconName: icon.name
}
