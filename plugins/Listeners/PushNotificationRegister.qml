import QtQuick 2.9
import Observer 1.0
import RequestHttp 1.0

Item {
    id: rootItem
    objectName: "PushNotificationRegister"

    Component.onCompleted: console.log(rootItem.objectName + " created!!")
    Component.onDestruction: console.log(rootItem.objectName + " deleted!")

    property string token: Utils.readFirebaseToken()
    onTokenChanged: {
        if (!token || !window.userProfile || !window.userProfile.profile.id) {
            return
        } else if (window.userProfile.profile.push_notification_token === token) {
            // rootItem.destroy()
            return
        } else {
            var params = JSON.stringify({
                "id": window.userProfile.profile.id,
                "push_notification_token": rootItem.token
            })
            console.log("sending Firebase token to webservice...")
            requestHttp.post("/token_register/", params)
        }
    }

    RequestHttp {
        id: requestHttp
        onFinished: {
            if (statusCode === 200) {
                Subject.notify(Config.events.setUserProperty, {"key": "push_notification_token", "value": rootItem.token})
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
        events: [Config.events.userProfileChanged, Config.events.newPushNotificationToken]
        onUpdated: rootItem.token = Utils.readFirebaseToken()
        Component.onCompleted: Subject.attach(tokenRegisterObserver1, event)
    }
}
