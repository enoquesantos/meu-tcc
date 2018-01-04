import QtQuick 2.8
import QtQuick.Layouts 1.3
import QtQuick.Controls 2.2
import QtGraphicalEffects 1.0

ItemDelegate {
    id: listItem
    objectName: "ListItem.qml"
    width: parent.width; height: 60
    antialiasing: true; opacity: enabled ? 1 : 0.7
    anchors.horizontalCenter: parent.horizontalCenter
    visible: secondaryLabelText.length || primaryLabelText.length

    Binding {
        target: listItem
        when: visible
        property: "height"
        value: 60
    }

    Binding {
        target: listItem
        when: !visible
        property: "height"
        value: 0
    }

    Rectangle {
        id: backgroundRec
        anchors.fill: parent
        color: selected ? selectedBackgroundColor : backgroundColor

        Behavior on color {
            ColorAnimation { duration: 200 }
        }
    }

    property int margins: 12

    property Rectangle separator

    property alias primaryLabel: _primaryLabel
    property alias primaryLabelText: _primaryLabel.text
    property color primaryLabelColor: "#444"
    property alias primaryIconName: primaryActionIcon.name
    property alias primaryIconColor: primaryActionIcon.color
    property alias primaryImageSource: primaryActionImage.imgSource

    property alias secondaryActionItem: secondaryAction
    property alias secondaryLabel: _secondaryLabel
    property alias secondaryLabelText: _secondaryLabel.text
    property alias secondaryIconName: secondaryActionIcon.name
    property alias secondaryIconColor: secondaryActionIcon.color
    property alias secondaryImageSource: secondaryActionImage.imgSource

    property bool selected: false
    property bool showIconBold: false
    property bool showShadow: false
    property bool showSeparator: true
    property bool showSeparatorInset: false

    property color backgroundColor: "transparent"
    property color selectedTextColor: primaryLabelColor
    property color selectedBackgroundColor: Qt.rgba(0,0,0,0.1)

    property int badgeBorderWidth: 0
    property color badgeTextColor: "#444"
    property color badgeBorderColor: "transparent"
    property color badgeBackgroundColor: "transparent"
    property string badgeText
    property bool badgeInRightSide: false
    property real badgeWidth: listItem.height * 0.70

    signal secondaryActionClicked(var mouse)
    signal secondaryActionPressAndHold(var mouse)

    // add a shadow at item round area
    // to show a shadow, the property 'showShadow' needs to be true
    Loader {
        asynchronous: true; active: showShadow
        onLoaded: item.parent = parent
        sourceComponent: DropShadow {
            z: -1; radius: 4; samples: 9; color: "#000000"
            width: parent.width-1; height: parent.height-1
        }
    }

    // define a Badge component to uses as item action.
    // the badge can be placed in right or left side (default)
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

    // the item content with primary and secondary text.
    RowLayout {
        id: row
        spacing: margins; width: listItem.width; height: listItem.height
        implicitWidth: width; implicitHeight: height
        anchors { left: parent.left; right: parent.right; margins: listItem.margins }

        Item {
            id: primaryAction
            width: visible ? 40 : 0; height: parent.height
            visible: (primaryIconName.length > 0 || primaryImageSource.length > 0 || badgeText.length > 0)

            Loader {
                id: primaryActionLoader
                asynchronous: true
                anchors.centerIn: parent
                sourceComponent: badgeComponent
                active: badgeText.length > 0
                onLoaded: { item.parent = primaryAction; primaryActionIcon.visible = false }
            }

            RoundedImage {
                id: primaryActionImage
                width: imgSource ? parent.width * 0.70 : 0; height: width
                anchors { verticalCenter: parent.verticalCenter; horizontalCenter: parent.horizontalCenter }
            }

            Connections {
                target: primaryActionImage
                onImageReady: primaryActionIcon.visible = false
            }

            AwesomeIcon {
                id: primaryActionIcon
                size: _primaryLabel.font.pointSize*1.4
                color: _primaryLabel.color; clickEnabled: false
                width: name ? parent.width * 0.70 : 0; height: width; weight: showIconBold ? Font.Bold : Font.Light
                anchors { verticalCenter: parent.verticalCenter; horizontalCenter: parent.horizontalCenter }
            }

            MouseArea {
                anchors.fill: parent
                onClicked: listItem.clicked(mouse)
                onPressAndHold: listItem.pressAndHold(mouse)
            }
        }

        Column {
            spacing: 0
            width: parent.width - (primaryAction.width + secondaryAction.width)
            height: parent.height - 4
            anchors {
                left: primaryAction.right
                leftMargin: primaryAction.visible ? margins : 0
                right: secondaryAction.left
                rightMargin: secondaryAction.visible ? margins : 0
            }

            Item {
                id: _firstItemText
                width: parent.width; height: secondaryLabelText.length ? parent.height/2 : parent.height

                Text {
                    id: _primaryLabel
                    width: parent.width; height: parent.height
                    color: selected ? selectedTextColor : primaryLabelColor
                    font { weight: Font.Bold; pointSize: 15 }
                    elide: Text.ElideRight; wrapMode: Text.WrapAnywhere
                    verticalAlignment: secondaryLabelText.length ? 0 : Text.AlignVCenter
                    anchors {
                        top: _secondaryLabel.text ? parent.top : undefined
                        topMargin: _secondaryLabel.text ? Qt.platform.os === "linux" || Qt.platform.os === "osx" ? 6 : 8 : 0
                        verticalCenter: !_secondaryLabel.text ? parent.verticalCenter : undefined
                    }
                }
            }

            Item {
                width: _firstItemText.width; height: _secondaryLabel.text ? _firstItemText.height : 0

                Text {
                    id: _secondaryLabel
                    width: _primaryLabel.width; height: _primaryLabel.height
                    color: _primaryLabel.color; opacity: 0.7
                    elide: Text.ElideRight; wrapMode: Text.WrapAnywhere
                    font { weight: Font.DemiBold; pointSize: 12 }
                    verticalAlignment: Text.AlignVCenter
                }
            }
        }

        Item {
            id: secondaryAction
            width: visible ? 40 : 0; height: parent.height; anchors.right: parent.right
            visible: secondaryActionLoader.active || secondaryActionImage.visible || secondaryActionIcon.visible

            Loader {
                id: secondaryActionLoader
                anchors.centerIn: parent
                sourceComponent: badgeComponent
                asynchronous: true
                active: badgeText.length > 0 && badgeInRightSide && !secondaryActionImage.visible && !secondaryActionIcon.visible
                onLoaded: {
                    item.height = width
                    item.parent = secondaryAction
                    item.width = secondaryAction.width * 0.70
                }
            }

            RoundedImage {
                id: secondaryActionImage
                width: visible ? parent.width * 0.70 : 0; height: width
                visible: imgSource.length > 0
                anchors { verticalCenter: parent.verticalCenter; horizontalCenter: parent.horizontalCenter }
            }

            AwesomeIcon {
                id: secondaryActionIcon
                color: _primaryLabel.color
                width: visible ? parent.width * 0.70 : 0; height: width; clickEnabled: false
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

    // draw a line as item separator at bottom, can be usefult when used in ListView.
    // to show a separator, the property 'showSeparator' needs to be true
    Loader {
        asynchronous: true; active: showSeparator
        sourceComponent: Rectangle {
            antialiasing: true
            color: Qt.rgba(0,0,0,0.1)
            width: parent.width - x; height: 1
            anchors { bottom: parent.bottom; bottomMargin: 0; leftMargin: 0 }
            x: showSeparatorInset ? primaryAction.implicitWidth : 0
        }
        onLoaded: {
            item.parent = listItem
            listItem.separator = item
        }
    }
}
