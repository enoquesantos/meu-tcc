pragma Singleton

import QtQuick 2.8

FontLoader {
    id: _source
    source: Qt.resolvedUrl("Font.otf")

    property alias font: _source.name
}
