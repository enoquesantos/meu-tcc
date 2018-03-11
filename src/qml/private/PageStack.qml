import QtQuick 2.9
import QtQuick.Controls 2.2
import Observer 1.0

StackView {
    id: _pageStack
    anchors.fill: visible ? parent : undefined

    QtObject {
        id: internal
        property int count
        property Page _page
    }

    function pushIfNotExists(pageAbsPath, prop) {
        // if current active page is the required page, return.
        if (currentItem && currentItem.absPath === pageAbsPath)
            return
        internal.count = 0
        while (internal.count < depth) {
            internal._page = get(internal.count, StackView.ForceLoad)
            if (internal._page && internal._page.absPath === pageAbsPath)
                internal._page = pop(internal._page)
            else
                internal._page = null
            internal.count++
        }
        if (!internal._page)
            push(pageAbsPath, prop)
    }

    Observer {
        id: observer
        objectName: "pageStackObserver"
        event: Config.events.popCurrentPage
        onUpdated: {
            if (_pageStack.depth > 1)
                _pageStack.pop()
            else if (swipeView && swipeView.count && _pageStack.depth == 1)
                _pageStack.clear()
        }

        Component.onCompleted:  Subject.attach(observer, event)
    }
}
