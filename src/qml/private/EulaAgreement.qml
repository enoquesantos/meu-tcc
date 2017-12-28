import QtQuick 2.8
import QtQuick.Layouts 1.3
import QtQuick.Controls 2.1

BasePage {
    id: page
    objectName: "EulaAgreement.qml"
    title: qsTr("Terms of Use")
    hasListView: false
    hasNetworkRequest: false

    Flickable {
        id: flickable
        clip: true; interactive: true
        boundsBehavior: Flickable.StopAtBounds
        width: parent.width; height: parent.height - buttons.height
        contentHeight: eulaText.height+25; contentWidth: parent.width

        ScrollBar.vertical: ScrollBar { id: scrollBar }

        Keys.onUpPressed: scrollBar.decrease()
        Keys.onDownPressed: scrollBar.increase()

        // eula text generated in: http://eulatemplate.com/generator
        Text {
            id: eulaText
            width: parent.width
            text: Utils.readFile(":/app-eula.html")
            textFormat: Text.RichText
            wrapMode: Text.WordWrap
            font.capitalization: Font.AllUppercase
            horizontalAlignment: Text.AlignJustify
            anchors { left: parent.left; leftMargin: 20; right: parent.right; rightMargin: 20 }
        }
    }

    Rectangle {
        id: buttons
        width: parent.width; height: 60
        color: Config.theme.pageBackgroundColor; opacity: 0.9
        anchors { bottom: parent.bottom; bottomMargin: 0; left: parent.left; right: parent.right }

        CustomButton {
            text: qsTr("OK")
            visible: !Config.forceEulaAgreement
            anchors.centerIn: parent
            onClicked: {
                App.saveSetting("app_eula_accepted", "1")
                App.notifyEvent("app_eula_accepted", 0)
            }
        }

        CustomButton {
            implicitWidth: 120
            visible: Config.forceEulaAgreement
            text: qsTr("NOT AGREE")
            anchors { left: parent.left; leftMargin: 20; verticalCenter: parent.verticalCenter; horizontalCenter: undefined }
            onClicked: functions.alert(qsTr("Warning!"), qsTr("You need to read and accept the terms! Otherwise you will cannot use the application!!"), function() { return; }, function() { Qt.quit(); })
        }

        CustomButton {
            implicitWidth: 120
            text: qsTr("AGREE")
            visible: Config.forceEulaAgreement
            anchors { right: parent.right; rightMargin: 10; verticalCenter: parent.verticalCenter; horizontalCenter: undefined }
            onClicked: {
                App.saveSetting("app_eula_accepted", "1")
                App.eventNotify(Config.events.eulaAccepted, 0)
                window.setActivePage()
            }
        }
    }
}
