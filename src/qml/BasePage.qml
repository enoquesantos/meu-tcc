import QtQuick 2.9
import QtQuick.Controls 2.2
import RequestHttp 1.0

Page {
    id: basePage
    width: window.width
    background: Rectangle {
        id: _pageBackgroundRec
        anchors.fill: parent; color: Config.theme.pageBackgroundColor
    }

    // This property can be used to show buttons in window ToolBar
    // when layout is StackView. The toolbar look for this property to
    // add dynamic buttons into a row layout.
    //    property var toolBarButtons: [
    //        {
    //            "iconName": "gear",
    //            "callback": function() {
    //                console.log("clicou::gear")
    //            }
    //        },
    //        {
    //            "iconName": "calendar",
    //            "callback": function() {
    //                console.log("clicou::calendar")
    //            }
    //        },
    //        {
    //            "iconName": "ellipsis_v",
    //            "submenu": [
    //                {
    //                    "text": qsTr("Show in grid"),
    //                    "callback": function () { console.log("cickou!!!") }
    //                },
    //                {
    //                    "text": qsTr("Open drawer"),
    //                    "callback": function () { console.log("cickou!!!") }
    //                }
    //            ]
    //        }
    //    ]
    property var toolBarButtons: []

    // used by toolbar when page is on the pageStack to make bind with action button
    // if page set to 'goback' value, the toolbar show a arrow-left icon as back button
    // when user click in this button, the page is poped from pageStack.
    property string toolBarState: "normal"

    // keeps the page absolute path to bind with menu item (turning selected id the page is active).
    // is used too by pageStack to check if the page is already on the stack, moving to top if true.
    property string absPath: ""

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
    property bool isActivePage: window.currentPage && window.currentPage.absPath === absPath

    // each page can set a custom color to page background using this alias
    property alias pageBackgroundColor: _pageBackgroundRec.color

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

    // if the page not set 'hasNetworkRequest' to false, this loader create a
    // new instance of RequestHttp to page handle requests like get, post and upload or download files.
    // To page manage results from requests, needs to create a connection with requestHttp property or
    // with signal requestFinished(int, variant).
    Loader {
        asynchronous: false; active: hasNetworkRequest
        sourceComponent: RequestHttp {
            onFinished: requestFinished(statusCode, response)
            onError: {
                // signal signature: void error(int statusCode, const QVariant &message);
                if (statusCode !== 3)
                    return
                // alert is a signal on Main.qml.
                // on iOS, the alert show a dialog with a native appearence,
                // snackbar is a object on Main.qml most used in Android to show short warnings in android.
                if (["ios","osx"].indexOf(Qt.platform.os) > -1)
                    functions.alert(qsTr("Error!"), qsTr("Cannot connect to server!"))
                else
                    snackbar.show(qsTr("Cannot connect to server!"))
            }
        }
        onLoaded: {
            requestHttp = item
            isPageBusy = Qt.binding(function() { return item.status === RequestHttp.Loading })
        }
    }

    // if the page not set 'hasListView' to false,
    // this loader create a instance of ListView
    Loader {
        active: hasListView; asynchronous: false; anchors.fill: parent
        sourceComponent: CustomListView { }
        onLoaded: {
            listView = item
            listView.delegate = listViewDelegate
            listViewModel = item.listModel
        }
    }
}
