import QtQuick
import QtQuick.Controls

ApplicationWindow {
    color: 'transparent'
    // color: '#0000ff30' // Semi-transparent blue background for debugging

    height: minimumHeight
    minimumHeight: 600
    minimumWidth: 800
    visible: true
    width: minimumWidth
    // flags: Qt.FramelessWindowHint

    Item {
        anchors.fill: parent
        focus: true

        Keys.onPressed: event => {
            console.log('key pressed')
            if (event.key === Qt.Key_Escape) {
                mainMenu.visible = !mainMenu.visible;
                event.accepted = true;
            }
        }

        MainMenu {
            id: mainMenu

            anchors.fill: parent
            z: 1
            // visible: false
        }
    }
}

