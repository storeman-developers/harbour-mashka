import QtQuick 2.0
import Sailfish.Silica 1.0


MenuItem {
    enabled: !mmodel.busy
    //% "Rescan"
    text: qsTrId("mashka-rescan")
    onClicked: mmodel.reset()
}
