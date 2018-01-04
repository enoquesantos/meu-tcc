import QtQml 2.2
import QtQuick 2.8
import QtQuick.Layouts 1.3
import QtQuick.Controls 2.1
import Qt.labs.calendar 1.0

Popup {
    id: _datepicker
    objectName: "Datepicker.qml"
    modal: true; focus: true
    x: window.width / 2 - width / 2
    y: window.height * 0.05
    width: window.width * 0.80; height: window.height * 0.80

    property var dateObject: new Date()
    property int currentDay: dateObject.getDay()
    property int currentMonth: dateObject.getMonth()
    property int currentYear: dateObject.getFullYear()
    property var monthsList: []
    readonly property var currentLocale: Qt.locale(window.locale.name)

    Component.onCompleted: {
        for (var i = 0; i < 12; ++i)
            monthsList.push(currentLocale.standaloneMonthName(i, Locale.ShortFormat))
        monthsList = monthsList
    }

    /**
     * Emit the selected date after user click in OK buton.
     * To connect with this signal:
     *   datepicker.dateSelected.connect(function(day,month,year) {
     *       console.log("Date selected:")
     *       console.log("day,month,year: ", day + "/" + month + "/" + year)
     *   })
     */
    signal dateSelected(int day, int month, int year)

    Component {
        id: monthNameComp

        Rectangle {
            color: selected ? Config.theme.colorPrimary : nextYearRect.color
            radius: 4; width: 48; height: width-10

            property int index: 0
            property bool selected: index === _datepicker.currentMonth

            Behavior on color {
                ColorAnimation { duration: 150 }
            }

            Text {
                color: Config.theme.textColorPrimary
                text: monthsList.length > index ? monthsList[index] : ""
                anchors.centerIn: parent
                font.bold: parent.selected
            }

            MouseArea {
                anchors.fill: parent
                onClicked: _datepicker.currentMonth = index
            }
        }
    }

    // datepicker content
    Column {
        id: column
        spacing: 25
        anchors.fill: parent

        // years paginator
        RowLayout {
            id: row
            width: _datepicker.width * 0.95
            anchors.horizontalCenter: parent.horizontalCenter

            Rectangle {
                id: previousYearRect
                width: previousYear.implicitWidth * 2.5; height: 40
                color: Config.theme.colorControlHighlight
                radius: 4; anchors.left: parent.left

                AwesomeIcon {
                    id: previousYearIcon
                    name: "chevron_left"; opacity: 0.7
                    width: 25; height: width; color: previousYear.color
                    anchors { left: parent.left; verticalCenter: parent.verticalCenter }
                }

                Label {
                    id: previousYear
                    color: Config.theme.textColorPrimary
                    text: _datepicker.currentYear-1
                    font { pointSize: 12; bold: true }
                    anchors {
                        left: previousYearIcon.right
                        leftMargin: 7
                        verticalCenter: parent.verticalCenter
                    }
                }

                ToolTip {
                    id: previousYearTooltip
                    delay: 50
                    timeout: 3000
                    text: qsTr("Previous year")
                }

                MouseArea {
                    anchors.fill: parent
                    hoverEnabled: true
                    onClicked: _datepicker.currentYear--
                    onPressAndHold: {
                        previousYearTooltip.open()
                        _datepicker.currentYear -= 10
                    }
                }
            }

            Label {
                text: _datepicker.currentYear
                color: Config.theme.textColorPrimary
                font { pointSize: 22; bold: true }
                anchors.horizontalCenter: parent.horizontalCenter
            }

            Rectangle {
                id: nextYearRect
                color: previousYearRect.color; radius: 4
                width: nextYear.implicitWidth * 2.5; height: 40
                anchors.right: parent.right

                Label {
                    id: nextYear
                    text: _datepicker.currentYear+1; color: previousYear.color
                    font { pointSize: 12; bold: true }
                    anchors {
                        right: nextYearIcon.left
                        rightMargin: 7
                        verticalCenter: parent.verticalCenter
                    }
                }

                AwesomeIcon {
                    id: nextYearIcon
                    name: "chevron_right"; opacity: 0.7
                    width: 25; height: width; color: nextYear.color
                    anchors { right: parent.right; verticalCenter: parent.verticalCenter }
                }

                ToolTip {
                    id: nextYearTooltip
                    delay: 50
                    timeout: 3000
                    text: qsTr("Next year")
                }

                MouseArea {
                    anchors.fill: parent
                    hoverEnabled: true
                    onClicked: _datepicker.currentYear++
                    onPressAndHold: {
                        nextYearTooltip.open()
                        _datepicker.currentYear += 10
                    }
                }
            }
        }

        // show a grig with months names
        GridLayout {
            id: gridMonth
            rows: 2; columns: 6
            columnSpacing: 7; rowSpacing: 15
            width: row.width
            anchors { left: parent.left; right: parent.right; margins: 0 }

            Component.onCompleted: {
                for (var i = 0; i <= _datepicker.monthsList.length; ++i)
                    monthNameComp.createObject(gridMonth, {"index":i})
            }
        }

        // show a grid with day name
        DayOfWeekRow {
            id: dayListRow
            locale: grid.locale
            width: row.width; Layout.fillWidth: true
            anchors.horizontalCenter: parent.horizontalCenter
            delegate: Text {
                color: Config.theme.textColorPrimary
                text: model.shortName
                font: dayListRow.font
                horizontalAlignment: Text.AlignHCenter
                verticalAlignment: Text.AlignVCenter
            }
        }

        // show a grid with month days
        MonthGrid {
            id: grid
            spacing: 10
            locale: _datepicker.currentLocale
            width: parent.width; Layout.fillWidth: true
            month: _datepicker.currentMonth
            year: _datepicker.currentYear
            anchors.horizontalCenter: parent.horizontalCenter
            delegate: Rectangle {
                color: isSelectedDay ? Config.theme.colorPrimary : "transparent"; radius: 4
                width: 48; height: 38

                Behavior on color {
                    ColorAnimation { duration: isSelectedDay ? 150 : 0 }
                }

                property bool isSelectedDay: parseInt(textDelegate.text) === currentDay && model.month === grid.month

                MouseArea {
                    anchors.fill: parent
                    onClicked: if (textDelegate.enabled) currentDay = parseInt(textDelegate.text)
                }

                Text {
                    id: textDelegate
                    text: model.day; color: Config.theme.textColorPrimary
                    font.bold: parent.isSelectedDay
                    enabled: model.month === grid.month
                    opacity: enabled ? 1.0 : 0.0
                    verticalAlignment: Text.AlignVCenter
                    horizontalAlignment: Text.AlignHCenter
                    anchors.centerIn: parent

                    Behavior on color {
                        ColorAnimation { duration: 150 }
                    }
                }
            }
        }
    }

    // datepicker buttons
    Row {
        spacing: 10
        anchors { bottom: parent.bottom; bottomMargin: -5; right: parent.right; rightMargin: 5 }

        Button {
            text: qsTr("CANCEL")
            font.bold: true
            highlighted: false
            flat: true
            onClicked: _datepicker.close()
        }

        Button {
            text: qsTr("OK")
            font.bold: true
            highlighted: false
            flat: true
            onClicked: {
                _datepicker.dateSelected(currentDay, _datepicker.currentMonth+1, _datepicker.currentYear)
                _datepicker.close()
            }
        }
    }
}
