import QtQuick 2.9
import Observer 1.0
import RequestHttp 1.0

import "qrc:/publicComponentes/" as Components

Item {
    id: rootItem
    objectName: "PushNotificationRegister"

    property string token
    onTokenChanged: if (token) sendTokenToServer()

    Component.onCompleted: console.log(rootItem.objectName + " created_> "+ Config.events.newPushNotificationToken)
    Component.onDestruction: console.log(rootItem.objectName + " onDestruction!")

    signal sendTokenToServer()
    onSendTokenToServer: {
        if (!window.userProfile || !window.userProfile.profile.id) {
            return
        } else if (window.userProfile.profile.push_notification_token && window.userProfile.profile.push_notification_token === rootItem.token) {
            rootItem.destroy()
            return
        }
        var params = JSON.stringify({
            "id": window.userProfile.profile.id,
            "push_notification_token": rootItem.token
        })
        console.log("sending Firebase token to webservice...")
        requestHttp.post("/token_register/", params)
    }

    RequestHttp {
        id: requestHttp
        onFinished: {
            if (statusCode === 200) {
                Subject.notify(Config.events.setUserProperty, {"key": "push_notification_token", "value": rootItem.token})
                App.removeSetting("pushNotificationToken")
                rootItem.destroy()
            }
        }
    }

    // observe a signal to sent the token to webserveice after registered by Firebase Service.
    // this action is useful when firebase token is registered after user logged in.
    // when token are updated, if user is logged in, we need to sent the token to webservice.
    Observer {
        id: tokenRegisterObserver1
        objectName: rootItem.objectName
        event: Config.events.newPushNotificationToken
        onUpdated: rootItem.token = eventData
        Component.onCompleted: Subject.attach(tokenRegisterObserver1, event)
    }

    // observe a signal to sent the token to webserveice after registered by Firebase Service.
    // this action is useful when firebase token is registered before user logged in.
    Observer {
        id: tokenRegisterObserver2
        objectName: rootItem.objectName
        event: Config.events.userProfileChanged
        onUpdated: rootItem.token = App.readSetting("pushNotificationToken", App.SettingTypeString)
        Component.onCompleted: Subject.attach(tokenRegisterObserver2, event)
    }
}
