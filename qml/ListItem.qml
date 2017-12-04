import QtQuick 2.8
import QtQuick.Layouts 1.3
import QtQuick.Controls 2.1
import QtQuick.Controls.Material 2.1

import "Awesome/"

Rectangle {
    id: listItem
    width: parent.width; height: visible ? 55 : 0
    antialiasing: true; opacity: enabled ? 1 : 0.6
    color: selected ? selectedBackgroundColor : "transparent"
    anchors.horizontalCenter: parent.horizontalCenter
    visible: secondaryLabelText.length || primaryLabelText.length

    Behavior on color {
        ColorAnimation { duration: 200 }
    }

    property int margins: 16
    property bool showIconBold: false

    property Rectangle separator
    property int separatorInset: primaryAction.visible ? 50 : 0

    property alias primaryLabel: _primaryLabel
    property alias primaryLabelText: _primaryLabel.text
    property color primaryLabelColor: "#444"
    property alias primaryIconName: primaryActionIcon.name
    property alias primaryIconColor: primaryActionIcon.color
    property alias primaryImageSource: primaryActionImage.imgSource

    property alias secondaryLabel: _secondaryLabel
    property alias secondaryLabelText: _secondaryLabel.text
    property alias secondaryIconName: secondaryActionIcon.name
    property alias secondaryIconColor: secondaryActionIcon.color
    property alias secondaryImageSource: secondaryActionImage.imgSource

    property bool selected: false
    property bool interactive: true
    property bool showShadow
    property bool showSeparator: true

    property alias backgroundColor: listItem.color
    property color selectedTextColor: primaryLabelColor
    property color selectedBackgroundColor: Qt.rgba(0,0,0,0.1)

    property int badgeBorderWidth: 0
    property color badgeTextColor: "#444"
    property color badgeBorderColor: "transparent"
    property color badgeBackgroundColor: "transparent"
    property string badgeText
    property bool badgeInRightSide: false
    property real badgeWidth: listItem.height * 0.80

    signal clicked(var mouse)
    signal pressAndHold(var mouse)
    signal secondaryActionClicked(var mouse)
    signal secondaryActionPressAndHold(var mouse)

    MouseArea {
        id: listItemMouseArea
        anchors.fill: parent
        onClicked: listItem.clicked(mouse)
        onPressAndHold: listItem.pressAndHold(mouse)
    }

    // to enable shadow, add follow line in main.cpp (after QGuiApplication object):
    // QQuickStyle::setStyle(QStringLiteral("Material"));
    Loader {
        asynchronous: true; active: showShadow
        onLoaded: item.parent = parent
        sourceComponent: Pane {
            z: -1
            width: parent.width-1; height: parent.height-1
            Material.elevation: 1
        }
    }

    Component {
        id: badgeComponent

        Rectangle {
            radius: 200; color: badgeBackgroundColor
            width: badgeWidth; height: width
            border { width: badgeBorderWidth; color: badgeBorderColor }
            anchors { verticalCenter: parent.verticalCenter; horizontalCenter: parent.horizontalCenter }

            Text {
                anchors.centerIn: parent
                text: badgeText; color: badgeTextColor
                font { weight: Font.DemiBold; pointSize: 12 }
            }
        }
    }

    RowLayout {
        id: row
        spacing: 15; width: listItem.width; height: listItem.height
        implicitWidth: width; implicitHeight: height
        anchors { left: parent.left; right: parent.right; margins: listItem.margins }

        Item {
            id: primaryAction
            width: visible ? 40 : 0; height: parent.height
            visible: (primaryIconName.length > 0 || primaryImageSource.length > 0 || badgeText.length > 0)

            Loader {
                id: primaryActionLoader
                anchors.centerIn: parent
                sourceComponent: badgeComponent
                asynchronous: true
                active: badgeText.length > 0
                onLoaded: { item.parent = primaryAction; primaryActionIcon.visible = false }
            }

            RoundedImage {
                id: primaryActionImage
                width: imgSource ? parent.width * 0.85 : 0; height: width
                anchors.verticalCenter: parent.verticalCenter
            }

            Connections {
                target: primaryActionImage
                onImageReady: primaryActionIcon.visible = false
            }

            Icon {
                id: primaryActionIcon
                size: _primaryLabel.font.pointSize*1.4
                color: _primaryLabel.color; clickEnabled: false
                width: name ? parent.width * 0.75 : 0; height: width; weight: showIconBold ? Font.Bold : Font.Light
                anchors { verticalCenter: parent.verticalCenter; horizontalCenter: parent.horizontalCenter }
            }

            MouseArea {
                anchors.fill: parent
                onClicked: listItem.clicked(mouse)
                onPressAndHold: listItem.pressAndHold(mouse)
            }
        }

        Column {
            width: parent.width - (primaryAction.width + secondaryAction.width)
            height: parent.height
            anchors {
                left: primaryAction.right
                leftMargin: primaryAction.visible ? 16 : 0
                right: secondaryAction.left
                rightMargin: secondaryAction.visible ? 16 : 0
            }

            Rectangle {
                id: _firstItemText
                color: "transparent"
                width: parent.width; height: secondaryLabelText.length ? 27 : 55

                Text {
                    id: _primaryLabel
                    width: parent.width; height: parent.height
                    color: selected ? selectedTextColor : primaryLabelColor
                    font { weight: Font.Bold; pointSize: 15 }
                    wrapMode: Text.WrapAnywhere
                    elide: Text.ElideRight
                    verticalAlignment: secondaryLabelText.length ? 0 : Text.AlignVCenter
                    anchors {
                        top: _secondaryLabel.text ? parent.top : undefined
                        topMargin: _secondaryLabel.text ? 7 : 0
                        verticalCenter: !_secondaryLabel.text ? parent.verticalCenter : undefined
                    }
                }
            }

            Rectangle {
                color: "transparent"
                width: _firstItemText.width; height: _secondaryLabel.text ? _firstItemText.height : 0

                Text {
                    id: _secondaryLabel
                    width: parent.width * 0.90; height: parent.height
                    color: _primaryLabel.color; opacity: 0.7
                    elide: Text.ElideRight; wrapMode: Text.WrapAnywhere
                    font { weight: Font.DemiBold; pointSize: 11 }
                    verticalAlignment: Text.AlignVCenter
                }
            }
        }

        Item {
            id: secondaryAction
            width: visible ? 40 : 0; height: parent.height; anchors.right: parent.right
            visible: (secondaryActionLoader.active || secondaryActionImage.visible || secondaryActionIcon.visible)

            Loader {
                id: secondaryActionLoader
                anchors.centerIn: parent
                sourceComponent: badgeComponent
                asynchronous: true
                active: badgeText.length > 0 && badgeInRightSide && !secondaryActionImage.visible && !secondaryActionIcon.visible
                onLoaded: {
                    item.height = width
                    item.parent = secondaryAction
                    item.width = secondaryAction.width * 0.75
                }
            }

            RoundedImage {
                id: secondaryActionImage
                width: parent.width * 0.75; height: width
                visible: imgSource.length > 0
            }

            Icon {
                id: secondaryActionIcon
                color: _primaryLabel.color
                width: parent.width * 0.75; height: width; clickEnabled: false
                visible: !secondaryActionImage.visible && name.length > 0; weight: showIconBold ? Font.Bold : Font.Light
                anchors { verticalCenter: parent.verticalCenter; horizontalCenter: parent.horizontalCenter }
            }

            MouseArea {
                anchors.fill: parent
                enabled: secondaryAction.visible
                onClicked: secondaryActionClicked(mouse)
                onPressAndHold: secondaryActionPressAndHold(mouse)
            }
        }
    }

    Loader {
        asynchronous: true; active: showSeparator
        onLoaded: {
            item.parent = listItem
            listItem.separator = item
        }
        sourceComponent: Component {
            Rectangle {
                antialiasing: true
                color: Qt.rgba(0,0,0,0.1)
                width: parent.width - x; height: 1
                anchors { bottom: parent.bottom; bottomMargin: 0; leftMargin: 0 }
                x: primaryAction.visible ? 0 : separatorInset
            }
        }
    }
}
