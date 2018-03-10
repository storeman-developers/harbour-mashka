import QtQuick 2.0
import Sailfish.Silica 1.0


Item {
    property bool visible2: true
    property alias title: titleLabel.text
    property alias dataSizeText: dataSizeLabel.text
    property alias appsCountText: appsCountLabel.text

    opacity: visible2 ? 1.0 : 0.0
    visible: opacity === 1.0
    width: page.orientation === Orientation.Portrait ?
               parent.width : (parent.width - Theme.horizontalPageMargin * 2) / content.columns
    height: titleLabel.height + dataSizeLabel.height + appsCountLabel.height

    Behavior on opacity { FadeAnimation { } }

    Label {
        id: titleLabel
        width: parent.width
        horizontalAlignment: Qt.AlignHCenter
        font.pixelSize: Theme.fontSizeExtraLarge
        color: Theme.highlightColor
        wrapMode: Text.WordWrap
    }

    Label {
        id: dataSizeLabel
        anchors.top: titleLabel.bottom
        width: parent.width
        horizontalAlignment: Qt.AlignHCenter
        color: Theme.highlightColor
        wrapMode: Text.WordWrap
    }

    Label {
        id: appsCountLabel
        visible: text
        anchors.top: dataSizeLabel.bottom
        width: parent.width
        horizontalAlignment: Qt.AlignHCenter
        color: Theme.highlightColor
        wrapMode: Text.WordWrap
    }
}
