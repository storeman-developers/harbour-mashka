import QtQuick 2.0
import Sailfish.Silica 1.0


Label {
    property int value: 0
    property string label: ""

    width: parent.width
    text: label + ": " +
          (value > 0 ? prettyBytes(value) :
                       //% "empty"
                       qsTrId("mashka-empty"))
    font.pixelSize: Theme.fontSizeTiny
    color: delegate.highlighted ?
               Theme.highlightColor : value > 0 ? Theme.secondaryColor :
                                                  Theme.secondaryHighlightColor
}
