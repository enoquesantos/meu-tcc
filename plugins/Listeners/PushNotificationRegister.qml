import QtQuick 2.8

import "qrc:/publicComponentes/" as Components

Item {
    id: rootItem
    objectName: "PushNotificationRegister.qml"

    property string token

    // Component.onCompleted: console.log(rootItem.objectName + " created!")
    // Component.onDestruction: console.log(rootItem.objectName + " onDestruction!")

    signal sendTokenToServer()
    onSendTokenToServer: {
        if (!window.userProfile || !window.userProfile.profile.id)
            return
        if (!rootItem.token)
            rootItem.token = Utils.readFirebaseToken()
        if (!rootItem.token || window.userProfile.profile.push_notification_token && window.userProfile.profile.push_notification_token === rootItem.token) {
            rootItem.destroy()
            return
        }
        var params = JSON.stringify({
            "id": window.userProfile.profile.id,
            "push_notification_token": token
        })
        requestHttp.post("/token_register/", params)
        params = null
    }

    Components.RequestHttp {
        id: requestHttp
        onFinished: {
            if (statusCode === 200) {
                App.eventNotify(Config.events.setUserProfileData, {"key": "push_notification_token", "value": rootItem.token})
                App.removeSetting(Config.events.newPushNotificationToken)
                rootItem.destroy()
            }
        }
    }

    // this action is necessary when firebase token is generated or updated after user logged in.
    // the token will be registered in another application process started by firebase service.
    // when token are updated, if user is logged in, we need to send the token to webservice.
    Connections {
        target: App
        onEventNotify: {
            // signal signature: eventNotify(QString eventName, QVariant eventData)
            if (eventName === Config.events.newPushNotificationToken) {
                rootItem.token = eventData
                sendTokenToServer()
            } else if (eventName === Config.events.userProfileChanged) {
                rootItem.token = App.readSetting(Config.events.newPushNotificationToken, App.STRING)
                sendTokenToServer()
            }
        }
    }
}
