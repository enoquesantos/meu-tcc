import QtQuick 2.8

Item {
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
            value: Config.usesSwipeView && !pageStack.depth ? swipeView.currentItem : pageStack.currentItem
        }
    }

    // load a Binding object to create a bind with TabBar and current active page in swipeView.
    // The TabBar, after created, keeps a reference to window.footer and needs
    // to be visible when pageStack is empty or if current page set showTabBar to true.
    Loader {
        asynchronous: true
        active: Config.usesSwipeView
        sourceComponent: Binding {
            when: window.footer != null
            target: window.footer
            property: "visible"
            value: !pageStack.depth && swipeView.currentItem && swipeView.currentItem.showTabBar
        }
    }
}
