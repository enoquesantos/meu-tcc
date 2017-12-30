import QtQuick 2.8

Item {
    visible: false
    objectName: "UserProfile.qml"

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
            setProperty("image_path", "qrc:/default_user_image.svg")
        else if (profile.image_path.indexOf("file:/") < 0 && profile.image_path.indexOf("qrc:/") < 0 && profile.image_path.indexOf("http") < 0)
            setProperty("image_path", Config.restService.baseImagesUrl + profile.image_path)

        // save the new user profile in local settings
        App.saveSetting("user_profile_data", profile)

        // notify pages and components that make bind with user profile
        App.eventNotify(Config.events.userProfileChanged, null)

        // if profileName is not defined, set the user permission name
        if (!profileName)
            profileName = profile.roles.description
    }

    // set a new property value in user profile
    function setProperty(key, value) {
        profile[key] = value
        profile = profile
    }

    // this signal is used to set user profile data
    // and is called by login page after a login success response
    signal setProfile(var newUserData)
    onSetProfile: {
        if (newUserData && "user" in newUserData && "email" in newUserData.user) {
            if ("program" in newUserData)
                newUserData.user.program = newUserData.program
            profile = newUserData.user
            isLoggedIn = true
            newUserData = {}
            delete newUserData
        } else {
            isLoggedIn = false
            App.saveSetting("user_profile_data", "")
        }
        // after user login or logout,
        // the application page needs to be updated
        window.setActivePage()
    }
}
