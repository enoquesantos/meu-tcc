import QtQuick 2.9
import QtQuick.Controls 2.2

Page {
    id: page

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

        Button {
            text: qsTr("OK")
            visible: !Config.forceEulaAgreement
            anchors.centerIn: parent
            onClicked: {
                App.saveSetting("eula_accepted", "1")
                functions.setActivePage()
            }
        }

        Button {
            implicitWidth: 120
            visible: Config.forceEulaAgreement
            text: qsTr("NOT AGREE")
            anchors { left: parent.left; leftMargin: 20; verticalCenter: parent.verticalCenter; horizontalCenter: undefined }
            onClicked: functions.alert(qsTr("Warning!"), qsTr("You need to read and accept the terms! Otherwise you will cannot use the application!!"), function() { return; }, function() { Qt.quit(); })
        }

        Button {
            implicitWidth: 120
            text: qsTr("AGREE")
            visible: Config.forceEulaAgreement
            anchors { right: parent.right; rightMargin: 10; verticalCenter: parent.verticalCenter; horizontalCenter: undefined }
            onClicked: {
                App.saveSetting("eula_accepted", "1")
                window.setActivePage()
            }
        }
    }
}
