import QtQuick 2.9
import QtMultimedia 5.8

BasePage {
    id: cameraCapturePage
    title: qsTr("Capture a image")
    toolBarState: "goBack"
    hasListView: false; hasNetworkRequest: false
    pageBackgroundColor: "black"
    isPageBusy: countdowToHide.running || countdowToStart.running

    function capture() {
        camera.searchAndLock()
        camera.imageCapture.capture()
        countdowToHide.start()
    }

    function close() {
        camera.stop()
        // remove page from the stack
        Subject.notify(Config.events.popCurrentPage, null)
    }

    Timer {
        id: countdowToStart
        interval: 750; running: true
        onTriggered: camera.start()
    }

    Timer {
        id: countdowToHide
        interval: 1300
        onTriggered: close()
    }

    Camera {
        id: camera
        imageProcessing.whiteBalanceMode: CameraImageProcessing.WhiteBalanceFlash
        flash.mode: Camera.FlashRedEyeReduction
        exposure {
            exposureCompensation: -1.1
            exposureMode: Camera.ExposurePortrait
        }
        imageCapture {
            onImageCaptured: photoPreview.source = preview
            onImageSaved: Subject.notify(Config.events.cameraImageSaved, "file://" + path)
        }
    }

    Image {
        id: photoPreview
        z: 0; mirror: true
        smooth: true; asynchronous: true
        width: window.width * 1.175; height: window.height * 1.175; clip: true
        fillMode: Image.PreserveAspectFit
        anchors { top: parent.top; topMargin: 0; bottom: parent.bottom; bottomMargin: 0 }

        VideoOutput {
            z: 0; visible: true
            focus: visible // to receive focus and capture key events when visible
            source: camera
            orientation: -90
            anchors.fill: parent
            height: photoPreview.height
            fillMode: VideoOutput.PreserveAspectFit

            MouseArea {
                z: 0
                anchors.fill: parent
                onClicked: capture()
            }
        }
    }

    Rectangle {
        z: 1
        color: "#000"; opacity: 0.8
        width: window.width; height: 50
        anchors { bottom: parent.bottom; bottomMargin: 0 }

        AwesomeIcon {
            z: 2
            color: "#fafafa"; width: 50
            anchors { left: parent.left; leftMargin: 15; verticalCenter: parent.verticalCenter }
            name: "refresh"; size: 26
            onClicked: camera.position = camera.position == Camera.BackFace ? Camera.FrontFace : Camera.BackFace
        }

        AwesomeIcon {
            z: 2
            color: "#fafafa"; width: 50
            anchors.centerIn: parent
            name: "camera"; size: 26
            onClicked: capture()
        }

        AwesomeIcon {
            z: 2
            color: "#fafafa"; width: 50
            anchors { right: parent.right; rightMargin: 15; verticalCenter: parent.verticalCenter }
            name: "picture_o"; size: 26
            onClicked: {
                FileDialog.open()
                close()
            }
        }
    }
}
