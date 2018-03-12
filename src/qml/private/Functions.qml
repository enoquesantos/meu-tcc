import QtQuick 2.9

QtObject {
    /**
      * append a new page to SwipeView container (if application uses SwipeView)
      * and create a new TabBarButton to window.footer (a instance of QuickControls TabBar)
      *
      * @page needs to be a object like a page from plugin config.json.
      * @isDynamicallyAdded is a boolean value that can be passed to add the page and the button
      * in specific position, using the "order" value defined by page object.
      */
    function addNewMenuItem(page, isDynamicallyAdded) {
        if (!page.title || !page.absPath)
            return
        // if application uses drawer, add the pages to drawer list options
        if (!Config.usesSwipeView) {
            var userProfileName = window.userProfile.profileName
            if (page.showInDrawer && !Config.hasLogin || (page.showInDrawer && Config.hasLogin && userProfileName && page.roles.indexOf(userProfileName) > -1))
                window.drawer.listViewModel.insert(isDynamicallyAdded ? page.order : window.drawer.listViewModel.count, page)
        } else {
            // if application uses SwipeView, for each plugin page, create a page and TabBarButton
            if ((page.showInTabBar && !Config.hasLogin) || (page.showInTabBar && Config.hasLogin && window.userProfile.profileName && page.roles.indexOf(window.userProfile.profileName) > -1)) {
                var comp = Qt.createComponent(page.absPath)
                // if the page has a some error, continue to next page
                if (comp.status === Component.Error) {
                    console.error(comp.errorString())
                    return
                }
                if (isDynamicallyAdded)
                    swipeView.insertItem(page.order, comp.createObject(swipeView))
                else
                    swipeView.addItem(comp.createObject(swipeView))
                comp = Qt.createComponent(Qt.resolvedUrl("TabBarButton.qml"))
                if (comp.status === Component.Error) {
                    console.error(item.errorString())
                    return
                }
                comp = comp.createObject(window.footer, {"checked": isDynamicallyAdded ? !page.order : !window.footer.count, "text": page.title, "iconName": page.awesomeIcon})
                if (isDynamicallyAdded)
                    window.footer.insertItem(page.order, comp)
                else
                    window.footer.addItem(comp)
            }
        }
    }

    /**
      * if Config.usesSwipeView is true (in config.json),
      * load all pages and add a button for each page into window tabbar (window footer) using page icon, page title and
      * create a new QuickControls Page object to append in swipeView container.
      * The window footer in this case, keeps a instance of TabBar.
      */
    function setMenuItens() {
        var i = 0, length = 0, pages = []
        pages = App.readSetting("pages", App.SettingTypeJsonArray)
        length = pages.length
        while (i < length) addNewMenuItem(pages[i++], false)
    }

    /**
      * this function can be used to show a user alert dialog with a native look and feel.
      * @title a string with dialog title
      * @message a string with dialog message
      * @acceptCallback can be a javascript function, called after user accept the message
      * @rejectCallback can be a javascript function, called after dialog closed by cancel button
      */
    function alert(title, message, acceptCallback, rejectCallback) {
        dialog.title = title || ""
        dialog.text = message || ""
        if (!dialog.text)
            return
        if (acceptCallback)
            dialog.accepted.connect(acceptCallback)
        else
            dialog.accepted.connect(function() { }) // prevent call the last connected callback
        if (rejectCallback)
            dialog.rejected.connect(rejectCallback)
        else
            dialog.rejected.connect(function() { }) // prevent call the last connected callback
        dialog.open()
    }

    /**
      * buttonPressed is a function to handle the Android back button and prevent close the app.
      * If the platform is iOS, this signal is ignored.
      *
      * The signal is emited by Keys.onBackPressed and not exists on iOS.
      * When user click in android backButton, some rules needs to be verify:
      * 1: if the alert dialog is opened, will be closed.
      * 2: if swipeView is active, and pagestack is visible in the moment,
      *    pop the current page from pageStack.
      * 3: if swipeView is active, pageStack is empty and current page is not the first page on the swipe container,
      *    paginate to previous page using the decrementIndex function from swipeview.
      * 4: if swipe view is not active and pageStack has more of one page,
      *    pop() and destroy the current page from stack.
      * 5: if swipe view is not active and pageStack has only one page, or swipeview is active and current page is
      *    the first item, minimize the app calling the java activiy moveTaskToBack() via bind with java activity from App QObject.
      * @e a Event QML object
      */
    function buttonPressed(e) {
        if (Qt.platform.os !== "android") {
            e.accepted = true
            return
        } else if (dialog.visible) {
            dialog.close()
        } else if (pageStack.depth && !currentPage.lockGoBack) {
            // when uses swipe view, the pagestack is a secondary container and we need to call pagestack.clear()
            // to remove the current page to focus in swipeView. This needed when the stackView has only a one page!
            if (pageStack.depth === 1 && swipeView && swipeView.count > 0)
                pageStack.clear()
            else if (pageStack.depth === 1 && (!swipeView || !swipeView.count))
                Utils.minimizeWindow()
            else
                pageStack.pop()
        } else if (!pageStack.depth && swipeView && swipeView.count && footer.currentIndex) {
            swipeView.decrementCurrentIndex()
        } else {
            // App.minize call a android java to move app to background, but keep running
            Utils.minimizeWindow()
        }
        // setting button.accepted to false, prevent the ApplicationWindow to be closed
        e.accepted = false
    }

    /**
      * first method called by application window to set the first visible page to the user.
      * Some rules needs to check:
      * 1: If is Config.hasLogin and user logged in, check if application is configured to use TabBar,
      *    if true clear pagestack, used by previous page like login.
      *
      * 2: If application is not configured to use TabBar, replace current page.
      * 3: If user is not logged in, load the login page from saved settings.
      *    Login page needs to be set by some plugin and the path is saved by PluginManager.
      */
    function setActivePage() {
        if (Config.showEula && App.readSetting("eula_accepted") !== "1") {
            pageStack.push(Qt.resolvedUrl("EulaAgreement.qml"))
            return
        }
        if (!Config.hasLogin || Config.hasLogin && userProfile && userProfile.isLoggedIn) {
            if (Config.usesSwipeView) {
                pageStack.clear()
                setMenuItens()
            } else {
                pageStack.replace(App.readSetting("homePageUrl", App.SettingTypeString))
            }
        } else {
            // get the login page url defined by some plugin
            var loginPageUrl = App.readSetting("loginPageUrl", App.SettingTypeString)
            // if pageStack has more of one item like logout page,
            // replace the last page by login page to prevent user back to
            // previous page using the android back button
            if (pageStack.depth > 0)
                pageStack.replace(loginPageUrl)
            else
                pageStack.push(loginPageUrl)
        }
    }

    /**
     * load application listeners, a list of qml files
     * defined by plugins.
     */
    function loadListeners() {
        var component = {}, listeners = App.readSetting("listeners", App.SettingTypeJsonArray)
        while (listeners.length) {
            component = Qt.createComponent(listeners.pop())
            if (component.status === Component.Ready)
                component.createObject(window, {"visible":false})
            else
                console.error("Listener Component Error: ", component.errorString())
        }
    }
}
