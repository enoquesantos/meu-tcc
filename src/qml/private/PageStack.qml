import QtQuick 2.8
import QtQuick.Controls 2.1

StackView {
    id: _pageStack
    objectName: "PageStack.qml"
    anchors.fill: visible ? parent : undefined

    property int count
    property Page _page

    function pushIfNotExists(pageAbsPath, prop) {
        // if current active page is the required page, return.
        if (currentItem && currentItem.absPath === pageAbsPath)
            return
        for (count = 0; count < depth; ++count) {
            _page = get(count, StackView.ForceLoad)
            if (_page.absPath === pageAbsPath)
                _page = pop(_page)
            else
                _page = null
        }
        if (!_page)
            push(pageAbsPath, prop)
    }

    Connections {
        target: App
        onEventNotify: {
            if (eventName === Config.events.goBack) {
                if (_pageStack.depth > 1)
                    _pageStack.pop()
                else if (swipeView && swipeView.count && _pageStack.depth == 1)
                    _pageStack.clear()
            }
        }
    }
}
