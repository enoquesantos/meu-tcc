import QtQuick 2.9

Item {
    visible: false

    // create a Binding object to create a bind with ToolBar and current active page in stackView.
    Binding {
        when: window.header != null
        target: window.header
        property: "visible"
        value: pageStack.depth && pageStack.currentItem && "showToolBar" in pageStack.currentItem && pageStack.currentItem.showToolBar
    }

    // create a Binding object to create a bind with PageStack
    // turnning visible when PageStack is not empty.
    Binding {
        target: pageStack
        property: "visible"
        value: pageStack.depth > 0
    }

    // create a Binding object to create a bind with window.currentPage.
    // If the app uses SwipeView, the currentItem point to currentPage in swipeView, otherwise point to
    // currentPage in StackView. Some QML objects make binds with window.currentPage like the toolbar.
    Binding {
        target: window
        property: "currentPage"
        value: Config.usesSwipeView && !pageStack.depth ? swipeView.currentItem : pageStack.currentItem
    }

    // create a Binding object to create a bind with TabBar and current active page in swipeView.
    // The TabBar, after created, keeps a reference to window.footer and needs
    // to be visible when pageStack is empty or if current page set showTabBar to true.
    Binding {
        when: Config.usesSwipeView && window.footer != null
        target: window.footer
        property: "visible"
        value: !pageStack.depth && swipeView && swipeView.currentItem && "showTabBar" in swipeView.currentItem && swipeView.currentItem.showTabBar
    }
}
