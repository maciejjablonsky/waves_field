import QtQuick

Rectangle {
    property color b: "red" // borders color


    anchors.fill: parent
    border.color: b
    border.width: 1
    color: "transparent"
}