import QtQuick 2.0
import Sailfish.Silica 1.0
import harbour.mashka 1.0


Dialog {
    id: dialog
    canAccept: switch2.checked

    onStatusChanged: {
        if (status === DialogStatus.Closed && result === DialogResult.Accepted) {
            mmodel.deleteUnusedData(mashka.processConfigEnabled ?
                                       MModel.AllData :
                                       MModel.CacheData | MModel.LocalData)
        }
    }

    DialogHeader {
        id: header
    }

    SilicaFlickable {
        anchors {
            top: header.bottom
            left: parent.left
            right: parent.right
            bottom: parent.bottom
        }
        contentHeight: content.height

        Column {
            id: content
            width: parent.width
            spacing: Theme.paddingMedium

            Label {
                id: label
                anchors {
                    left: parent.left
                    right: parent.right
                    margins: Theme.horizontalPageMargin
                }
                color: Theme.highlightColor
                wrapMode: Text.WordWrap
                //% "<h3>Warning!</h3><br />"
                //% "<p>This action can damage your device. You accept it solely at your own risk!</p>"
                text: qsTrId("mashka-confirmation-warning")
            }

            TextSwitch {
                id: switch1
                //% "I understand the risks"
                text: qsTrId("mashka-understand1")
            }

            TextSwitch {
                id: switch2
                visible: opacity > 0.0
                opacity: switch1.checked ? 1.0 : 0.0
                //% "I completely understand the risks"
                text: qsTrId("mashka-understand2")

                onVisibleChanged: if (!visible) checked = false

                Behavior on opacity { FadeAnimation { } }
            }
        }
    }
}
