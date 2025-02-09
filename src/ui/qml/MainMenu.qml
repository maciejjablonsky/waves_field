import QtQuick
import QtQuick.Layouts
import QtQuick.Controls
import WfUI

Rectangle {
    anchors.fill: parent
    color: 'darkslategray'

    Rectangle {
        anchors.horizontalCenter: parent.horizontalCenter
        color: 'slategray'
        height: childrenRect.height
        width: 10 * Units.xxl

        ColumnLayout {
            Button {
                Layout.alignment: Qt.AlignHCenter
                text: "Pick simulation"

                onClicked: console.log('clicked simulation picker')
            }
            Button {
                text: "Settings"

                onClicked: console.log('clicked settings')
            }
        }
    }
}