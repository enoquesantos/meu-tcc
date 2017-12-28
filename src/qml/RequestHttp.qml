import RequestHttp 1.0 

RequestHttp {
    baseUrl: Config.restService.baseUrl
    basicAuthorizationUser: Config.restService.authentication.userName || ""
    basicAuthorizationPassword: Config.restService.authentication.userPass || ""
    onError: {
        // signal signature: void error(int statusCode, const QVariant &message);
        if (statusCode !== 3)
            return
        // alert is a signal on Main.qml.
        // on iOS, the alert show a dialog with a native appearence,
        // snackbar is a object on Main.qml most used in Android to show short warnings in android.
        if (["ios", "osx"].indexOf(Qt.platform.os) > -1)
            functions.alert(qsTr("Error!"), qsTr("Cannot connect to server!"), null, null)
        else
            snackbar.show(qsTr("Cannot connect to server!"))
    }
}
