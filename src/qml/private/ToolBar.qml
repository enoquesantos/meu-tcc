import QtQuick 2.8
import QtQuick.Layouts 1.3
import QtQuick.Controls 2.1
import QtGraphicalEffects 1.0

ToolBar {
    id: toolBar
    objectName: "ToolBar.qml"
    visible: window.currentPage && window.currentPage.showToolBar && pageStack.depth > 0
    height: visible ? 55 : 0
    state: window.currentPage ? window.currentPage.toolBarState : ""
    states: [
        State {
            name: "normal"
            PropertyChanges { target: toolButtonFirst; iconName: "bars"; action: Config.events.openDrawer }
        },
        State {
            name: "goback"
            PropertyChanges { target: toolButtonFirst; iconName: "arrow_left"; action: Config.events.goBack }
        },
        State {
            name: "search"
            PropertyChanges { target: toolButtonFirst; iconName: "arrow_left"; action: Config.events.cancel }
        }
    ]

    property bool enableToolBarShadow: window.currentPage && "enableToolBarShadow" in window.currentPage && window.currentPage.enableToolBarShadow === true

    // if current page define a list of itens to submenu (the last item displayed in ToolBar),
    // the itens will be append into a dropdown list
    property bool hasMenuList: window.currentPage && window.currentPage.toolBarMenuList && window.currentPage.toolBarMenuList.length > 0

    // this property can be used by page to set a custom color to this ToolBar
    property string toolBarColor: Config.theme.colorPrimary
    onToolBarColorChanged: if (Qt.platform.os === "android" && toolBarColor !== Config.theme.colorPrimary) SystemStatusBar.color = toolBarColor

    // this property can be used by some page to change the color of all icons in the ToolBar
    // is read by each ToolBarButton set the icon color
    property color defaultTextColor: Config.theme.colorAccent

    /**
     * A object to the toolbar actions to the pages when is used with pageStack.
     * ToolBar offears 4 buttons put into a RowLayout from right to left.
     * Each button can be used to page add some events.
     * Each button needs the icon name and action name.
     * The action name is send to window eventNotify.
     * This Object needs to be a object like this:
     *    {
     *       "toolButton3": {"action":"delete", "icon":"trash"},
     *       "toolButton4": {"action":"copy", "icon":"copy"}
     *    }
     */
    property var toolBarActions: window.currentPage ? window.currentPage.toolBarActions : ({})

    Loader {
        onLoaded: toolBar.background = item
        asynchronous: false; active: true
        sourceComponent: Rectangle {
            color: toolBarColor; border.width: 0; radius: 0
            width: toolBar.width; height: toolBar.height
            layer {
                enabled: enableToolBarShadow
                effect: DropShadow {
                    samples: 17; radius: 12
                    color: "#4D000000"; spread: 0
                    verticalOffset: 1; horizontalOffset: 0
                }
            }
        }
    }

    Connections {
        target: window
        onCurrentPageChanged: {
            if ("toolBarColor" in window.currentPage)
                toolBarColor = window.currentPage.toolBarColor
            else if (Qt.platform.os === "android")
                SystemStatusBar.color = Config.theme.statusBarColor
            if (optionsToolbarMenu)
                optionsToolbarMenu.reset = true
            if (!hasMenuList)
                return
            var itens = window.currentPage.toolBarMenuList
            for (var i = 0; i < itens.length; ++i)
                optionsToolbarMenu.addItem(itens[i])
            delete itens
        }
    }

    RowLayout {
        id: toolBarItens
        anchors { fill: parent; leftMargin: 8; rightMargin: 8; top: parent.top; topMargin: 0 }

        ToolBarButton {
            id: toolButtonFirst
            iconColor: defaultTextColor
            onClicked: if (action === Config.events.cancel) toolBar.state = "normal"

            NumberAnimation on rotation {
                from: 0; to: 360; running: toolBar.state === "goback"
                duration: 350
            }
        }

        Item {
            id: title
            width: visible ? parent.width * 0.55 : 0; height: parent.height
            visible: toolBar.state !== "search"
            anchors { left: toolButtonFirst.right; leftMargin: 12; verticalCenter: parent.verticalCenter }

            Text {
                elide: Text.ElideRight
                text: window.currentPage && window.currentPage.title || ""; color: defaultTextColor
                anchors.verticalCenter: parent.verticalCenter
                font { weight: Font.DemiBold; pointSize: Config.fontSize.large }
            }
        }

        ToolBarSearch {
            id: searchToolbar
            visible: toolBar.state === "search"
            anchors { left: title.right; leftMargin: 4; bottom: parent.bottom; bottomMargin: -1 }
            onSearchTextChanged: window.currentPage.searchText = searchText
        }

        ToolBarButton {
            id: toolButton2
            iconColor: defaultTextColor
            anchors.right: toolButton3.left
            actionName: visible ? toolBarActions.toolButton2.action : ""
            iconName: visible ? toolBarActions.toolButton2.icon : ""
            visible: "toolButton2" in toolBarActions && toolBar.state !== "search"
        }

        ToolBarButton {
            id: toolButton3
            iconColor: defaultTextColor
            anchors.right: toolButton4.left
            actionName: visible ? toolBarActions.toolButton3.action : ""
            iconName: visible ? toolBarActions.toolButton3.icon : ""
            visible: "toolButton3" in toolBarActions && toolBar.state !== "search"
        }

        ToolBarButton {
            id: toolButton4
            iconColor: defaultTextColor
            anchors.right: toolButtonLast.left
            actionName: visible ? toolBarActions.toolButton4.action : ""
            iconName: visible ? toolBarActions.toolButton4.icon : ""
            visible: "toolButton4" in toolBarActions && toolBar.state !== "search"
            onClicked: if (action === "search") toolBar.state = "search"
        }

        ToolBarButton {
            id: toolButtonLast
            iconColor: defaultTextColor
            anchors.right: parent.right
            actionName: "submenu"; iconName: "ellipsis_v"
            visible: hasMenuList && toolBar.state !== "search"
            onClicked: optionsToolbarMenu.open()

            Menu {
                id: optionsToolbarMenu
                x: parent ? (parent.width - width) : 0
                transformOrigin: Menu.BottomRight

                property bool reset: false
                onResetChanged: {
                    for (var i = 0; i < optionsToolbarMenu.contentData.length; i++) {
                        optionsToolbarMenu.removeItem(0)
                        optionsToolbarMenu.removeItem(i)
                    }
                }
            }
        }
    }
}
