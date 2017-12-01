import QtQuick 2.8

import "qrc:/qml/"

Item {
    objectName: "HandleUserProfileImage.qml"

    // Component.onCompleted: console.log(objectName + " created!")
    // Component.onDestruction: console.log(objectName + " onDestruction!")

    RequestHttp {
        id: requestHttp
        onFinished: if (statusCode == 200 && "id" in response && user.profile.id === response.id) user.setProperty("image_path", response.image_path)
        onDownloadedFileSaved: user.setProperty("image_path", filePath.toString())
    }

    Connections {
        target: App
        onEventNotify: if (eventName === Config.events.userProfileChanged && user.profile.image_path.indexOf("http") > -1) requestHttp.downloadFile([user.profile.image_path], true)
    }

    // create a connection with system image gallery manager, instantiated in the main.cpp.
    // When user select a file, a signal is emitted with the chosen image path
    // and we need to send the resource to the webservice
    Connections {
        target: FileDialog
        onAccepted: {
            // if fileUrl, send the image as binary data to webservice
            if (!fileUrl)
                return
            var headers = ({})
            headers["Expires"] = "0"
            headers["Content-Disposition"] = "multipart/form-data; filename=upload.jpg"
            requestHttp.uploadFile("/imageUpload/" + user.profile.id + "/", [fileUrl], headers)
            user.setProperty("image_path", fileUrl)
            delete headers
        }
    }
}
