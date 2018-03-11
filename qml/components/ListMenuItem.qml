import QtQuick 2.0
import Sailfish.Silica 1.0


ListItem {
    property alias text: label.text
    property string iconSource
    readonly property color _color: {
        if (!enabled) {
            Theme.secondaryHighlightColor
        } else if (highlighted) {
            Theme.highlightColor
        } else {
            Theme.primaryColor
        }
    }

    id: item
    width: parent.width
    contentHeight: Math.max(icon.implicitHeight, label.implicitHeight) +
                   Theme.paddingSmall * 2

    Item {
        anchors {
            left: parent.left
            right: parent.right
            leftMargin: Theme.paddingSmall
            rightMargin: Theme.horizontalPageMargin
            verticalCenter: parent.verticalCenter
        }

        Image {
            id: icon
            anchors.verticalCenter: parent.verticalCenter
            source: iconSource ? (iconSource + "?" + _color) : ""
        }

        Label {
            id: label
            anchors {
                verticalCenter: parent.verticalCenter
                left: icon.right
                leftMargin: Theme.paddingMedium
                right: parent.right
            }
            color: _color
            wrapMode: Text.WordWrap
        }
    }
}
