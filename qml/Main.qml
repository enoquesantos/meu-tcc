import QtQuick 2.8
import QtQuick.Controls 2.1
import QtQuick.Window 2.2

ApplicationWindow {
    id: window
    visible: true
    width: Qt.platform.os === "linux" || Qt.platform.os === "osx" ? (Screen.width/2.5) : Screen.width; height: Qt.platform.os === "linux" || Qt.platform.os === "osx" ? (Screen.height * 0.90) : Screen.height
    title: currentPage ? (currentPage.title + " - " + Config.applicationName) : (Config.applicationName + " - " + Config.applicationDescription)
    onClosing: utils.buttonPressed(close)

    Component.onCompleted: {
        utils.setActivePage()
        // when runnig in desktop mode, centralize the application window.
        if (Qt.platform.os === "linux" || Qt.platform.os === "osx") {
            setX(Screen.width / 2 - width / 2)
            setY(Screen.height / 2 - height / 2)
        }
    }

    // keeps a instance of MessageDialog with native look and feel,
    // the instance will be created dinamically by platform type.
    property QtObject dialog

    // swipe view is used when app set 'usesTabBar' to true in config.json.
    // If true, the app pages can be changed by swiping or
    // using the TabBar buttons put in the bottom of window.
    property SwipeView swipeView

    // keeps a reference to the current visible page at the window.
    property QtObject currentPage: Config.usesTabBar ? (pageStack.depth > 0 ? pageStack.currentItem : swipeView.currentItem) : pageStack.currentItem

    // keeps a instance of UserProfile if the application has UserProfile
    // if "hasLogin" (from config.json) is defined to true.
    // All qml components can read the user information from "user" reference.
    property UserProfile userProfile

    // the first function called by window to set the first page to the user.
    // to more details take a look in utils.setActivePage()
    signal setActivePage()
    onSetActivePage: utils.setActivePage()

    // load a Binding object to create a bind with ToolBar and current active page in stackView.
    Loader {
        asynchronous: true
        active: true
        sourceComponent: Binding {
            when: window.header != null
            target: window.header
            property: "visible"
            value: pageStack.depth && pageStack.currentItem && pageStack.currentItem.showToolBar
        }
    }

    // load a Binding object to create a bind with PageStack
    // turnning visible when PageStack is not empty.
    Loader {
        asynchronous: true
        active: true
        sourceComponent: Binding {
            target: pageStack
            property: "visible"
            value: pageStack.depth > 0
        }
    }

    // load a Binding object to create a bind with window.currentPage.
    // If the app uses SwipeView, the currentItem point to currentPage in swipeView, otherwise point to
    // currentPage in StackView. Some QML objects make binds with window.currentPage like the toolbar.
    Loader {
        asynchronous: true
        active: true
        sourceComponent: Binding {
            target: window
            property: "currentPage"
            value: Config.usesTabBar && !pageStack.depth ? swipeView.currentItem : pageStack.currentItem
        }
    }

    // load a Binding object to create a bind with TabBar and current active page in swipeView.
    // The TabBar, after created, keeps a reference to window.footer and needs
    // to be visible when pageStack is empty or if current page set showTabBar to true.
    Loader {
        asynchronous: true
        active: Config.usesTabBar
        sourceComponent: Binding {
            when: window.footer != null
            target: window.footer
            property: "visible"
            value: !pageStack.depth && swipeView.currentItem && swipeView.currentItem.showTabBar
        }
    }

    // load the main TabBar to show pages buttons, used with swipeview if "usesTabBar" is true
    // in config.json. The app can uses the swipeView + tabBar to swap the application pages.
    Loader {
        active: Config.usesTabBar
        source: "TabBar.qml"
        onLoaded: footer = item
    }

    // load the ToolBar if the app is not configured to use TabBar.
    // The toolbar is used to show a button to open the navigation drawer (if "usesDrawer" is defined to true in config.json)
    // and dynamic buttons defined by each page, for some actions like show a submenu or search button.
    Loader {
        active: !Config.usesTabBar && (!Config.hasLogin || Config.hasLogin && user.isLoggedIn)
        source: "ToolBar.qml"
        onLoaded: header = item
    }

    // load the navigation drawer to be used as application menu
    // The Drawer is a instance of QML Drawer with some customizations
    // and is used to show the plugins pages and work with PageStack.
    Loader {
        asynchronous: true
        active: Config.usesDrawer && Config.hasLogin && user.isLoggedIn
        source: "Drawer.qml"
    }

    // load the UserProfile and point to the window userProfile
    // All qml components can read the user information from "user" reference.
    Loader {
        asynchronous: true
        active: Config.hasLogin
        source: "UserProfile.qml"
    }

    // load a new instance of messages dialog component,
    // using the platform name for best look and fell appearence.
    Loader {
        active: true
        source: Qt.platform.os === "android" ? "AndroidDialog.qml" : "Dialog.qml"
        onLoaded: dialog = item
    }

    // load a dynamic SwipeView container as the main page container,
    // if "usesTabBar" (from config.json) is defined to true.
    Loader {
        anchors.fill: active ? parent : undefined
        active: Config.usesTabBar
        sourceComponent: SwipeView {
            visible: pageStack.depth === 0
            anchors.fill: visible ? parent : undefined
            currentIndex: footer ? footer.currentIndex : 0
        }
        onLoaded: { window.swipeView = item }
    }

    // this component create all listeners plugins.
    // to listeners plugins get access to user profile data or currentPage
    // or any window object or property, the listeners needs to be loaded in this context.
    ListenersLoader { }

    // handle android back button,
    // used to pop pages when is pressed by user.
    Item {
        Keys.onBackPressed: utils.buttonPressed(event)
    }

    // keeps the window signals, modularized to reduce the Main.qml size. :)
    Utils {
        id: utils
    }

    // handle the android Snackbar widget,
    // used to show some application warnings.
    Snackbar {
        id: snackbar
        z: 1
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
