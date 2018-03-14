import QtQuick 2.0
import Sailfish.Silica 1.0


MouseArea {
    property alias text: label.text

    height: Theme.itemSizeSmall
    width: isPortrait ? Screen.width : Screen.height / 3

    Label {
        id: label
        anchors.centerIn: parent
        width: parent.width
        horizontalAlignment: Qt.AlignHCenter
        color: pressed ? Theme.highlightColor : Theme.primaryColor
    }
}
