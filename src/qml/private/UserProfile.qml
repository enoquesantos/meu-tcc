import QtQuick 2.9
import Observer 1.0

Item {
    id: _userProfile
    objectName: "UserProfile"; visible: false

    // this property is a shortcut to the user profile permission
    property string profileName

    property bool isLoggedIn: App.readSetting("is_user_logged_in", App.SettingTypeBool)
    onIsLoggedInChanged: App.saveSetting("is_user_logged_in", isLoggedIn)

    property var profile: App.readSetting("user_profile_data", App.SettingTypeJsonObject)
    onProfileChanged: {
        if (!profile || !Object.keys(profile).length)
            return

        // check user profile image
        if (!profile.image_path)
            internal.setProperty("image_path", "qrc:/default_user_profile.svg")
        else if (!internal.isLocalImagePath() && profile.image_path.indexOf("http") < 0)
            internal.setProperty("image_path", (Config.restService.baseImagesUrl || Config.restService.baseUrl) + profile.image_path)

        // save the new user profile in local settings
        App.saveSetting("user_profile_data", profile)

        // notify pages and components that make bind with user profile
        Subject.notify(Config.events.userProfileChanged, profile)

        // if profileName is not defined, set the user permission name
        if (!profileName)
            profileName = "permission" in profile && "name" in profile.permission ? profile.permission.name : ""
    }

    QtObject {
        id: internal

        function isLocalImagePath() {
            return profile.image_path.indexOf("file:/") > -1 || profile.image_path.indexOf("qrc:/") > -1
        }

        // set a new property value in user profile
        function setProperty(key, value) {
            if (!key || profile[key] === value) return
            profile[key] = value
            profile = profile
        }

        // this signal is used to set user profile data
        // and is called by login page after a login success response
        function initProfile(userData) {
            if (userData && "email" in userData) {
                profile = userData
                isLoggedIn = true
            } else {
                isLoggedIn = false
                App.saveSetting("user_profile_data", "")
            }
            // after user login or logout,
            // the application active page will be replaced
            window.setActivePage()
        }

        function updateProfile(userData) {
            if (userData && "email" in userData)
                profile = userData
        }
    }

    // observe a signal to init the user profile data.
    // This event can be sent by some plugin, after the user login
    Observer {
        id: observer1
        objectName: _userProfile.objectName
        event: Config.events.initUserProfile
        onUpdated: internal.initProfile(eventData)
        Component.onCompleted: Subject.attach(observer1, event)
    }

    // observe a signal to set or update some user profile property.
    // This event can be sent by profile edition page or object listener
    Observer {
        id: observer2
        objectName: _userProfile.objectName
        event: Config.events.setUserProperty
        onUpdated: internal.setProperty(eventData.key, eventData.value)
        Component.onCompleted: Subject.attach(observer2, event)
    }

    // observe a signal to clear the user profile data.
    // This event can be sent by a plugin, after user confirm logout
    Observer {
        id: observer3
        objectName: _userProfile.objectName
        event: Config.events.logoutUser
        onUpdated: internal.initProfile(false)
        Component.onCompleted: Subject.attach(observer3, event)
    }

    // observe a signal to update the user profile.
    // This event can be sent by profile edition page or object listener
    Observer {
        id: observer4
        objectName: _userProfile.objectName
        event: Config.events.updateUserProfile
        onUpdated: internal.updateProfile(eventData)
        Component.onCompleted: Subject.attach(observer4, event)
    }
}
