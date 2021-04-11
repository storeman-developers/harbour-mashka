import QtQuick 2.0
import Sailfish.Silica 1.0


Label {
    property real value: 0
    property string label: ""

    width: parent.width
    text: label + ": " +
          (value > 0 ? Format.formatFileSize(value) :
                       //% "empty"
                       qsTrId("mashka-empty"))
    font.pixelSize: Theme.fontSizeTiny
    color: delegate.highlighted ?
               Theme.highlightColor : value > 0 ? Theme.secondaryColor :
                                                  Theme.secondaryHighlightColor
}
