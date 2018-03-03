import QtQuick 2.8

Item {
    visible: false

    // this property is a shortcut to the user profile permission
    property string profileName

    property bool isLoggedIn: App.readSetting("is_user_logged_in", App.SettingTypeBool)
    onIsLoggedInChanged: App.saveSetting("is_user_logged_in", isLoggedIn)

    property var profile: App.readSetting("user_profile_data", App.SettingTypeJsonObject)
    onProfileChanged: {
        if (!profile || !Object.keys(profile).length)
            return

        // check user profile image
        if (!profile.image_url)
            internal.setProperty("image_url", "qrc:/default_user_image.svg")
        else if (!internal.isLocalImagePath() && profile.image_url.indexOf("http") < 0)
            internal.setProperty("image_url", (Config.restService.baseImagesUrl || Config.restService.baseUrl) + profile.image_url)

        // save the new user profile in local settings
        App.saveSetting("user_profile_data", profile)

        // notify pages and components that make bind with user profile
        App.eventNotify(Config.events.userProfileChanged, profile)

        // if profileName is not defined, set the user permission name
        if (!profileName)
            profileName = profile.user_role.name
    }

    QtObject {
        id: internal

        function isLocalImagePath() {
            return profile.image_url.indexOf("file:/") > -1 || profile.image_url.indexOf("qrc:/") > -1
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
            // the application page needs to be updated
            window.setActivePage()
        }
    }

    Connections {
        target: App
        onEventNotify: {
            if (eventName === Config.events.initUserProfile)
                internal.initProfile(eventData)
            else if (eventName === Config.events.setUserProfileData)
                internal.setProperty(eventData.key, eventData.value)
            else if (eventName === Config.events.logoutApplication)
                internal.initProfile(false)
        }
    }
}
