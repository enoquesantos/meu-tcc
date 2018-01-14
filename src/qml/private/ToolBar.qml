import QtQuick 2.8
import QtQuick.Layouts 1.3
import QtQuick.Controls 2.1
import QtGraphicalEffects 1.0

import "qrc:/src/qml/" as Components
import "qrc:/src/qml/private" as Private

ToolBar {
    id: toolBar
    objectName: "ToolBar.qml"
    visible: window.currentPage && window.currentPage.showToolBar && pageStack.depth > 0
    state: window.currentPage ? window.currentPage.toolBarState : "normal"
    height: visible ? 55 : 0

    // show a shadow effect when user flickable to bottom
    // and the ToolBar shadow is turn visible
    property bool enableToolBarShadow: window.currentPage && "enableToolBarShadow" in window.currentPage && window.currentPage.enableToolBarShadow === true

    // this property can be used by page to set a custom color to this ToolBar
    property string toolBarColor: Config.theme.colorPrimary
    onToolBarColorChanged: if (Qt.platform.os === "android" && toolBarColor !== Config.theme.colorPrimary) SystemStatusBar.color = toolBarColor

    // this property can be used by some page to change the color of all icons in the ToolBar
    // is read by each ToolBarButton set the icon color
    property color defaultTextColor: Config.theme.colorAccent

    Loader {
        onLoaded: toolBar.background = item
        asynchronous: false; active: true
        sourceComponent: Rectangle {
            color: toolBarColor; border.width: 0; radius: 0
            width: toolBar.width; height: toolBar.height
            layer {
                enabled: enableToolBarShadow && Config.applicationStyle.indexOf("Material") > -1
                effect: DropShadow {
                    samples: 17; radius: 12
                    color: "#4D000000"; spread: 0
                    verticalOffset: 1; horizontalOffset: 0
                }
            }
        }
    }

    /**
      * Pages can add buttons dynamically to toolbar, using a object array.
      * Each button needs a icon and a callback function.
      * If actionName is send, will be sent to App.eventNotify(actionName, null).
      * To add a buttons to ToolBar, the page needs to set a object like this:
      * property var toolBarButtons: [
      *       {
      *           iconName: "gear"
      *           callback: function () { ... }
      *       },
      *       {
      *           iconName: "calendar"
      *           callback: function () { ... }
      *       }
      * ]
      */
    Component {
        id: toolButonsComponent
        Components.ToolBarButton { }
    }

    /**
     * The Page can create a submenu in ToolBar and set menu-itens dinamically,
     * like a sub-menu of options. Needs to be a list of MenuItem properties:
     *     1: 'text' - the text to be displayed
     *     2: a callback to handle the 'onTriggered' action, that is triggered when user touch in the item.
     * toolBarMenuList: [
     *    {
     *        "text": qsTr("Show in grid")
     *        "callback": function () { ... }
     *    },
     *    {
     *        "text": qsTr("Check all")
     *        "callback": function () { ... }
     *    }
     * ]
     */
    Component {
        id: toolButonsMenu
        Menu { }
    }

    // MenuItem will be used to each item in submenu
    // if window.currentPage set a "mnenu" option in toolBarButtons list.
    Component {
        id: toolButonsMenuItem
        MenuItem { }
    }

    Connections {
        target: window
        onCurrentPageChanged: {
            // set the ToolBar color, if current page set a property color toolBarColor: "color_value"
            if ("toolBarColor" in window.currentPage && window.currentPage.toolBarColor.length) {
                toolBarColor = window.currentPage.toolBarColor
                // in android, set the System Status Bar to same color of ToolBar
                if (Qt.platform.os === "android")
                    SystemStatusBar.color = Config.theme.statusBarColor
            }
            // if previous page added buttons to toolbar,
            // all buttons needs to be removed, to put again!
            if (rowPageButtons.children.length)
                for (var i = 0; i < rowPageButtons.children.length; ++i)
                    rowPageButtons.children[i].destroy()
            // if current page set dynamic buttons, will be added to toolBar, else return.
            if (!window.currentPage.toolBarButtons) return
            var j, k, menu, menuItem, btn, pageButtons = window.currentPage.toolBarButtons
            for (j = 0; j < pageButtons.length; ++j) {
                btn = toolButonsComponent.createObject(rowPageButtons, {"iconColor": defaultTextColor, "iconName": pageButtons[j].iconName})
                if ("callback" in pageButtons[j])
                    btn.clicked.connect(pageButtons[j].callback)
                if ("submenu" in pageButtons[j]) {
                    menu = toolButonsMenu.createObject(rowPageButtons, {"x": -15, "y": 15})
                    btn.clicked.connect(function() { menu.open() })
                    for (k = 0; k < pageButtons[j].submenu.length; k++) {
                        menuItem = toolButonsMenuItem.createObject(toolBar, {"text": pageButtons[j].submenu[k].text})
                        menuItem.triggered.connect(pageButtons[j].submenu[k].callback)
                        menu.addItem(menuItem)
                    }
                }
                btn.parent = rowPageButtons
                rowPageButtons.children[j] = btn
            }
        }
    }

    Binding {
        target: toolBar
        property: "state"
        value: window.currentPage ? window.currentPage.toolBarState : "normal"
    }

    Binding {
        target: toolButtonFirst
        property: "actionName"
        value: Config.events.openDrawer
        when: toolBar.state === "normal"
    }

    Binding {
        target: toolButtonFirst
        property: "actionName"
        value: Config.events.popCurrentPage
        when: toolBar.state === "goBack"
    }

    Binding {
        target: toolButtonFirst
        property: "actionName"
        value: Config.events.cancel
        when: toolBar.state === "search"
    }

    Binding {
        target: toolButtonFirst
        property: "iconName"
        value: toolBar.state === "search" || toolBar.state === "goBack" ? "arrow_left" : "bars"
    }

    RowLayout {
        id: toolBarItens
        anchors { fill: parent; leftMargin: 8; rightMargin: 8; top: parent.top; topMargin: 0 }

        Components.ToolBarButton {
            id: toolButtonFirst
            iconColor: defaultTextColor
            // onClicked: if (actionName === Config.events.cancelSearch) toolBar.state = "normal"

            NumberAnimation on rotation {
                from: 0; to: 360; running: toolBar.state === "goBack"
                duration: 350
            }
        }

        Label {
            id: title
            elide: Label.ElideRight
            text: window.currentPage && window.currentPage.title || ""; color: defaultTextColor
            width: visible ? parent.width * 0.55 : 0; height: parent.height
            visible: toolBar.state !== "search"
            anchors { left: toolButtonFirst.right; leftMargin: 12; verticalCenter: parent.verticalCenter }
            font { weight: Font.DemiBold; pointSize: Config.fontSize.normal }
        }

        Private.ToolBarSearch {
            id: searchToolbar
            visible: toolBar.state === "search"
            anchors { left: title.right; leftMargin: 4; bottom: parent.bottom; bottomMargin: -1 }
            onSearchTextChanged: window.currentPage.searchText = searchText
        }

        RowLayout {
            id: rowPageButtons
            anchors.right: parent.right
        }
    }
}
