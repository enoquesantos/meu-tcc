import QtQuick 2.9
import QtQuick.Layouts 1.3
import QtQuick.Controls 2.2
import QtGraphicalEffects 1.0

import "qrc:/privateComponents" as Private

ToolBar {
    id: toolBar
    state: "normal"
    visible: window.currentPage && window.currentPage.showToolBar && pageStack.depth > 0
    height: visible ? 55 : 0

    // this property can be used by page to set a custom color to this ToolBar
    property color toolBarColor
    onToolBarColorChanged: {
        if (toolBarColor) {
            toolBar.background.color = toolBarColor
            // in android, set the System Status Bar to same color of ToolBar
            if (Qt.platform.os === "android")
                SystemStatusBar.color = toolBarColor
        }
    }

    /**
      * Pages can add buttons dynamically to toolbar, using a object array.
      * Each button needs a icon and a callback function.
      * If actionName is send, will be sent to Subject.notify(actionName, argument).
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
       id: toolBtnComponent
       Private.ToolBarButton { iconColor: title.color }
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
        id: toolBtnMenu
        Menu { }
    }

    // MenuItem will be used to each item in submenu
    // if window.currentPage set a "mnenu" option in toolBarButtons list.
    Component {
        id: toolBtnMenuItem
        MenuItem { }
    }

    Connections {
        target: window
        onCurrentPageChanged: {
            // set the ToolBar color, if current page has a property 'toolBarColor'
            if (window.currentPage && "toolBarColor" in window.currentPage)
                toolBarColor = window.currentPage.toolBarColor
            else
                toolBarColor = Config.theme.colorPrimary
            // if previous page add buttons in toolbar,
            // all buttons needs to be removed
            if (rowPageButtons.children.length)
                for (var i = 0; i < rowPageButtons.children.length; ++i)
                    rowPageButtons.children[i].destroy()
            // if current page not set dynamic buttons in toolBar, stop here.
            if (! "toolBarButtons" in window.currentPage || !window.currentPage.toolBarButtons.length) return
            var j, k, menu, menuItem, comp, btn, pageButtons = window.currentPage.toolBarButtons
            for (j = 0; j < pageButtons.length; ++j) {
                btn = pageButtons[j]
                comp = toolBtnComponent.createObject(rowPageButtons, {"iconName": btn.iconName})
                if ("callback" in btn)
                    comp.clicked.connect(btn.callback)
                if ("submenu" in btn) {
                    menu = toolBtnMenu.createObject(rowPageButtons, {"x": -60, "y": 15})
                    comp.clicked.connect(function() { menu.open() })
                    for (k = 0; k < btn.submenu.length; k++) {
                        menuItem = toolBtnMenuItem.createObject(toolBar, {"text": btn.submenu[k].text})
                        menuItem.triggered.connect(btn.submenu[k].callback)
                        menu.addItem(menuItem)
                    }
                }
                comp.parent = rowPageButtons
                rowPageButtons.children[j] = btn
            }
        }
    }

    Binding {
        target: toolBar
        property: "state"
        value: window.currentPage.toolBarState
        when: window.currentPage && "toolBarState" in window.currentPage
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
        value: Config.events.cancelSearch
        when: toolBar.state === "search"
    }

    Binding {
        target: toolButtonFirst
        property: "iconName"
        value: toolBar.state === "search" || toolBar.state === "goBack" ? "arrow_left" : "bars"
    }

    RowLayout {
        id: toolBarItens
        anchors { verticalCenter: parent.verticalCenter; left: parent.left; right: parent.right }

        Private.ToolBarButton {
            id: toolButtonFirst
            iconColor: title.color

            NumberAnimation on rotation {
                from: 0; to: 360; duration: 350
                running: toolBar.state === "goBack"
            }
        }

        Text {
            id: title
            elide: Text.ElideRight
            text: window.currentPage && window.currentPage.title || ""
            width: visible ? parent.width * 0.55 : 0; height: parent.height
            visible: toolBar.state !== "search"
            color: Config.theme.colorAccent
            anchors { left: toolButtonFirst.right; leftMargin: 16; verticalCenter: parent.verticalCenter }
            font { weight: Font.DemiBold; pointSize: Config.fontSize.large + 1 }
        }

        Private.ToolBarSearch {
            onSearchTextChanged: window.currentPage.searchText = searchText
        }

        RowLayout {
            id: rowPageButtons
            spacing: 0; anchors.right: parent.right
        }
    }
}
