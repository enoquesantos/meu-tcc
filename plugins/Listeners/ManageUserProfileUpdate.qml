import QtQuick 2.8

import "qrc:/qml/"

Item {
    objectName: "ManageUserProfileUpdate.qml"

    // Component.onCompleted: console.log(objectName + " created!")
    // Component.onDestruction: console.log(objectName + " onDestruction!")

    // object to manager user profile requests,
    // like update profile and image upload
    RequestHttp {
        id: requestHttp
        onFinished: {
            if (statusCode === 400 || statusCode === 403) {
                window.alert(qsTr("Error!"), qsTr("Error on update your profile! The email is already associated to another user!"), null, null)
            } else if (statusCode !== 200) {
                window.alert(qsTr("Error!"), qsTr("Cannot load response from the server! Please try again."), null, null)
            } else if ("id" in response && user.profile.id === response.id) {
                snackbar.show(qsTr("Profile was success updated!"))
                var newUser = ({})
                if (user.profile.program)
                    response.program = user.profile.program
                if (user.profile.course_sections)
                    response.course_sections = user.profile.course_sections
                user.profile = response
            }
        }
    }

    // to reduce the coupling, to update user profile is only done by a request signal emited by user profile edit page.
    // The page need to emit a signal with a new property values as a object sent in eventNotify signal parameters.
    // So, we need to send the new user profile to the webservice.
    // The update result is handle by onFinished at __requestHttp
    Connections {
        target: App
        onEventNotify: {
            // signal signature: eventNotify(string eventName, var eventData)
            if (eventName === Config.events.requestUpdateUserProfile && eventData && "email" in eventData)
                requestHttp.post("/update_user/", JSON.stringify(eventData))
            else if (eventName === Config.events.cameraImageSaved)
                FileDialog.copyFileToApplicationDir(eventData)
        }
    }
}
