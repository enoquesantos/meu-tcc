import QtQuick 2.8
import QtQuick.Controls 2.1
import QtQuick.Window 2.2

import "qrc:/src/qml/private"

ApplicationWindow {
    id: window
    visible: true
    width: Qt.platform.os === "linux" || Qt.platform.os === "osx" ? (Screen.width / 2.5) : Screen.width
    height: Qt.platform.os === "linux" || Qt.platform.os === "osx" ? (Screen.height * 0.90) : Screen.height
    title: currentPage ? (currentPage.title || qsTr("Welcome - %1".arg(Config.appName))) : "%1 - %2".arg(Config.appName).arg(Config.appDescription)
    onClosing: functions.buttonPressed(close)

    Component.onCompleted: {
        functions.setActivePage()
        // when runnig in desktop mode, centralize the application window.
        if (["linux","osx"].indexOf(Qt.platform.os) > -1) {
            setX(Screen.width / 2 - width / 2)
            setY(Screen.height / 2 - height / 2)
        }
    }

    // keeps a instance of MessageDialog with native look and feel,
    // the instance will be created dinamically by platform type.
    property QtObject dialog

    // swipe view is used when app set 'usesSwipeView' to true in config.json.
    // If true, the app pages can be changed by swiping or
    // using the TabBar buttons put in the bottom of window.
    property QtObject swipeView

    // keeps a reference to the current visible page at the window.
    property QtObject currentPage

    // keeps a instance of UserProfile if the application has UserProfile
    // if "hasLogin" (from config.json) is defined to true.
    // All qml components can read the user information from "user" reference.
    property QtObject userProfile

    // keeps a instance of Menu.qml where is a instance of QuickControls.Drawer
    // if "hasLogin" (from config.json) is defined to true.
    // All qml components can read the user information from "user" reference.
    property QtObject drawer

    // the first function called by window to set the first page to the user.
    // to more details take a look in functions.setActivePage()
    signal setActivePage()
    onSetActivePage: functions.setActivePage()

    // load the main TabBar to show pages buttons, used with swipeview if "usesSwipeView" is true
    // in config.json. The app can uses the swipeView + tabBar to swap the application pages.
    Loader {
        active: Config.usesSwipeView
        source: "TabBar.qml"
        onLoaded: footer = item
    }

    // load the ToolBar if the app is not configured to use TabBar.
    // The toolbar is used to show a button to open the navigation drawer (if "usesDrawer" is defined to true in config.json)
    // and dynamic buttons defined by each page, for some actions like show a submenu or search button.
    Loader {
        active: !Config.usesSwipeView && (!Config.hasLogin || Config.hasLogin && userProfile && userProfile.isLoggedIn)
        source: "ToolBar.qml"
        onLoaded: header = item
    }

    // load the navigation drawer to be used as application menu
    // The Drawer is a instance of QML Drawer with some customizations
    // and is used to show the plugins pages and work with PageStack.
    Loader {
        asynchronous: true
        active: Config.usesDrawer && Config.hasLogin && userProfile && userProfile.isLoggedIn
        source: "Drawer.qml"
        onLoaded: drawer = item
    }

    // load the UserProfile and point to the window userProfile
    // All qml components can read the user information from "user" reference.
    Loader {
        asynchronous: true
        active: Config.hasLogin
        source: "UserProfile.qml"
        onLoaded: userProfile = item
    }

    // load a new instance of messages dialog component,
    // using the platform name for best look and fell appearence.
    Loader {
        active: true
        source: Qt.platform.os === "android" ? "AndroidDialog.qml" : "Dialog.qml"
        onLoaded: dialog = item
    }

    // load a dynamic SwipeView container as the main page container,
    // if "usesSwipeView" (from config.json) is defined to true.
    Loader {
        anchors.fill: active ? parent : undefined
        active: Config.usesSwipeView
        sourceComponent: SwipeView {
            visible: pageStack.depth === 0
            anchors.fill: visible ? parent : undefined
            currentIndex: footer ? footer.currentIndex : 0
        }
        onLoaded: swipeView = item
    }

    // handle the android back button and is
    // used to pop pages when is pressed by user or,
    // paginate the SwipeView pages (calling decrementCurrentIndex())
    Item {
        Keys.onBackPressed: functions.buttonPressed(event)
    }

    // this component create all listeners plugins.
    // to listeners plugins get access to user profile data or currentPage
    // or any window object or property, the listeners needs to be loaded in this context.
    ListenersLoader { }

    // create the application bindings with main window widgets
    // like SwipeView with TabBar(window.footer) and StackView with ToolBar (window.header)
    Bindings { }

    // keeps the window signals, modularized to reduce the Main.qml size. :)
    Functions {
        id: functions
    }

    // handle the android Snackbar widget,
    // used to show some application warnings.
    Snackbar {
        id: snackbar
    }

    // handle the android Toast widget,
    // used to show some application information messages.
    Toast {
        id: toast
    }

    // the main page container, is aways available and QML pages can push or pop pages
    // using "pageStack" reference. This component has a simple customization, where
    // prevent push a new page if already exists in the stack.
    PageStack {
        id: pageStack
    }
}
