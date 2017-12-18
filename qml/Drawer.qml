import QtQuick 2.8
import QtQuick.Layouts 1.3
import QtQuick.Controls 2.1
import QtQuick.Controls.Material 2.1

Drawer {
    id: menu
    width: window.width * 0.85; height: window.height
    dragMargin: user.isLoggedIn ? Qt.styleHints.startDragDistance + 10 : 0

    Component.onCompleted: {
        // fix first open effect
        open()
        close()
    }

    property alias menuBackgroundColor: menuBackground.color
    property color menuItemSelectedTextColor: Qt.darker(menuBackgroundColor, 1.7)
    property color menuItemTextColor: Config.theme.textColorPrimary
    property alias userInfoBackgroundColor: userInfoRect.color
    property color userInfoTextColor: Config.theme.colorAccent

    // to reduce the couplin with anothers components, the Drawer menu listener events
    // from App and window. This connection handle two signals:
    //  1: a signal to open the drawer by event sent by toolbar
    //  2: a signal to uodate the user profile image
    Connections {
        target: App
        onEventNotify: {
            // signal signature: eventNotify(string eventName, var eventData)
            if (eventName === Config.events.openDrawer)
                open()
            else if (eventName === Config.events.refreshDrawerPages)
                listVIew.model = App.readSetting("pages", App.SettingTypeJsonArray)
            else if (eventName === Config.events.userProfileChanged && user.profile.hasOwnProperty("image_path"))
                drawerUserImageProfile.imgSource = user.profile.image_path
        }
    }

    // create a connection to handle the menu close after viewed page changed.
    Connections {
        target: window
        onCurrentPageChanged: close()
    }

    // set a background color to drawer menu, but the color can be
    // customized set a string color(rgb or color name) in 'menuBackgroundColor' property.
    Rectangle {
        id: menuBackground
        z: 0; anchors.fill: parent; color: Config.theme.colorAccent
    }

    // show a user profile image, with user name and email
    Rectangle {
        id: userInfoRect
        z: 2; color: Config.theme.menuBackgroundColor
        width: parent.width; height: 200
        anchors { top: parent.top; topMargin: 0; horizontalCenter: parent.horizontalCenter }

        RoundedImage {
            id: drawerUserImageProfile
            width: 100; height: width
            borderColor: Config.theme.colorAccent
            imgSource: user.profile.image_path
            anchors { top: parent.top; topMargin: 15; horizontalCenter: parent.horizontalCenter }
        }

        Text {
            color: userInfoTextColor; textFormat: Text.RichText
            text: user.profile.name + "<br><b>" + user.profile.email + "</b>"
            font.pointSize: Config.fontSize.normal; Layout.fillWidth: true
            horizontalAlignment: Text.AlignHCenter
            anchors {
                topMargin: 30
                top: drawerUserImageProfile.bottom
                horizontalCenter: parent.horizontalCenter
            }
        }
    }

    // the model "pages" is a list of objects (plugins) that set "showInDrawer" to true
    // each object has the follow properties:
    // {
    //    "absPath": "the qml file absolute path as string",
    //    "title": "the page title as string",
    //    "icon": "the icon name as string",
    //    "roles": [the list of roles],
    //    "order": integer,
    //    "isHome": boolean,
    //    "showInDrawer": boolean,
    //    "isLogin": boolean
    // }
    ListView {
        id: listVIew
        z: 1; width: parent.width
        model: App.readSetting("pages", App.SettingTypeJsonArray)
        anchors { top: userInfoRect.bottom; topMargin: 0; bottom: parent.bottom }
        boundsBehavior: Flickable.StopAtBounds
        ScrollIndicator.vertical: ScrollIndicator { }
        delegate: ListItem {
            visible: (modelData.showInDrawer && modelData.title && modelData.roles.length && modelData.roles.indexOf(user.profileName) > -1)
            primaryIconName: modelData.icon
            primaryLabelText: modelData.title
            primaryLabelColor: menuItemTextColor
            selected: window.currentPage && modelData.absPath === window.currentPage.absPath
            anchors { horizontalCenter: undefined; left: undefined; right: undefined }
            onClicked: {
                // if the clicked page already exists in the stackview, focus in the Page.
                // otherwise, create a new page and push in the stack.
                pageStack.pushIfNotExists(modelData.absPath, {"absPath":modelData.absPath})
                close()
            }
        }
    }
}
