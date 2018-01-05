import QtQuick 2.8
import QtQuick.Controls 2.1

import "."

Page {
    id: basePage
    objectName: "BasePage.qml"
    width: window.width
    background: Rectangle {
        id: _pageBackgroundRec
        anchors.fill: parent
        color: Config.theme.pageBackgroundColor
    }

    /**
     * A object with toolbar actions used when the page is on the pageStack
     * ToolBar offears 4 buttons put into a RowLayout from right to left.
     * Each button can be used to page add some events.
     * Each button needs the icon and action name propertys.
     * The action name is send to window eventNotify and the page
     * needs a connection to manager the clicks on each button.
     * This Object needs to be a object like this:
     *    {
     *       "toolButton3": {"action":"delete", "icon":"trash"},
     *       "toolButton4": {"action":"copy", "icon":"copy"}
     *    }
     */
    property var toolBarActions: ({})

    /**
     * This property is read by toolbar dinamically
     * This property can be used to page show a list of itens in last icon of toolbar
     * like a sub-menu of options. Needs to be a list of MenuItem with two propertys for each item:
     *     1: 'text' - the text to be displayed
     *     2: a event handle for onTriggered action, that is triggered when user touch in the item.
     * toolBarMenuList: [
     *    MenuItem {
     *        text: qsTr("Show in grid")
     *        onTriggered: listView.visible = !listView.visible
     *    },
     *    MenuItem {
     *        text: checkedAll ? qsTr("Uncheck all") : qsTr("Check all")
     *        onTriggered: { ... }
     *    }
     * ]
     */
    property list<MenuItem> toolBarMenuList

    // used by toolbar when page is on the pageStack to make bind with action button
    // if page set to 'goback' value, the toolbar show a arrow-left icon as back button
    // when user click in this button, the page is poped from pageStack.
    property string toolBarState: "normal"

    // keeps the page absolute path to bind with menu item (turning selected id the page is active).
    // is used too by pageStack to check if the page is already on the stack, moving to top if true.
    property string absPath: ""

    // flag to enable bottom shadow in app toolbar. Can be used to enable shadow
    // when user scroll the listView to down or page flickable to down (if page uses a Flickable)
    property bool enableToolBarShadow: false

    // pages can use this flag to prevent go to previous page (using android back button)
    // this flag will be check in Main.qml at connection with Keys.onBackPressed event
    property bool lockGoBack: false

    // flag to hide/show the application tabBar (window.footer), loaded in Main.qml
    // TabBar is loaded only if the value for property 'usesSwipeView' is set to true in settings.json
    // if TabBar is loaded, will be placed in the app footer and make a bind with SwipeView,
    // showing a page navigation buttons in window bottom to each application page
    property bool showTabBar: true

    // flag to hide/show the application toolBar (window.header), loaded in Main.qml
    // the toolbar is a custom component with a row of buttons
    // some pages like login or logout, hide the toolbar using this flag (setting to false)!
    property bool showToolBar: true

    // pages uses this flag to prevent object RequestHttp creation id not make http requests
    // the default is true and RequestHttp object is loaded asynchronously
    property bool hasNetworkRequest: true

    // to auto loader the listView
    property bool hasListView: true

    // if set to false, the busyIndicator will be put on the top
    property bool centralizeBusyIndicator: true

    // a flag to the current page state binding with requestHttp state
    property bool isPageBusy: false

    // keeps the status for current page
    // The currentPage is a window alias to currentItem in pageStack or swipeView
    property bool isActivePage: window.currentPage && window.currentPage.objectName === objectName

    // each page can set a custom color to page background using this alias
    property alias pageBackgroundColor: _pageBackgroundRec.color

    // set the text to actionMessage
    property string actionMessageText

    // set the awesome icon to actionMessage
    property string actionMessageIconName

    // a reference to busyIndicator, loaded if 'hasNetworkRequest'
    property BusyIndicator busyIndicator

    // a instance of RequestHttp created dynamically to page handle http requests
    // if the page not uses requests, set the property 'hasNetworkRequest' to false
    property RequestHttp requestHttp

    // by default, the page load a new ListView and set to this property
    // if the page not use or not need a list view, set the property 'hasListView' to false
    property ListView listView

    // a reference to listView model (QML ListModel instance)
    property ListModel listViewModel

    // component defined by child page to handle the ListView delegate
    property Component listViewDelegate

    // this signal is emited when user click in ActionMessage button.
    // the page can connect with this signal to reload page or make some action.
    // the ActionMessage is visible when page has ListView and the ListView Model is empty.
    signal requestUpdatePage()

    // this signal is emited after request http send a response
    signal requestFinished(int statusCode, var response)

    // show page loading element when request http is running
    Loader {
        asynchronous: false; active: hasNetworkRequest
        sourceComponent: BusyIndicator {
            parent: basePage
            visible: isPageBusy; z: 10
            anchors {
                centerIn: centralizeBusyIndicator ? parent : undefined
                top: centralizeBusyIndicator ? undefined : parent.top
                topMargin: centralizeBusyIndicator ? undefined : 20
                horizontalCenter: centralizeBusyIndicator ? undefined : parent.horizontalCenter
            }
        }
        onLoaded: busyIndicator = item
    }

    // page action message show a icon and a message into a column.
    // the action can be clicked by user sending the requestUpdatePage() signal
    Loader {
        asynchronous: false; active: hasListView
        sourceComponent: ActionMessage {
            visible: listView && listViewModel.count === 0 && !isPageBusy
            onClicked: requestUpdatePage()
        }
        onLoaded: {
            item.parent = basePage
            item.anchors.centerIn = parent
            if (basePage.actionMessageText)
                item.messageText = basePage.actionMessageText
            if (basePage.actionMessageIconName)
                item.iconName = basePage.actionMessageIconName
        }
    }

    // if the page not set 'hasNetworkRequest' to false, this loader create a
    // new instance of RequestHttp to page handle requests like get, post and upload or download files.
    // To page manage results from requests, needs to create a connection with requestHttp property or
    // with signal requestFinished(int, variant).
    Loader {
        asynchronous: false; active: hasNetworkRequest
        sourceComponent: RequestHttp {
            onFinished: requestFinished(statusCode, response)
        }
        onLoaded: {
            requestHttp = item
            isPageBusy = Qt.binding(function() { return item.status === RequestHttp.Loading })
        }
    }

    // if the page not set 'hasListView' to false,
    // this loader create a instance of ListView
    Loader {
        anchors.fill: parent
        asynchronous: false; active: hasListView
        sourceComponent: CustomListView { }
        onLoaded: {
            listView = item
            listView.delegate = listViewDelegate
            listViewModel = item._listModel
        }
    }
}
