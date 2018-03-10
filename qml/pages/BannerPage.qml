import QtQuick 2.0
import Sailfish.Silica 1.0
import "../components"


Page {
    id: page
    allowedOrientations: Orientation.All

    Label {
        anchors {
            top: parent.top
            topMargin: Theme.paddingLarge
            left: parent.left
            leftMargin: Theme.horizontalPageMargin
            right: parent.right
            rightMargin: Theme.horizontalPageMargin
        }
        color: Theme.highlightColor
        wrapMode: Text.WordWrap
        //% "<h3>Ahoy sailor!</h3><br />"
        //% "<p>You're using the application for some time and have already "
        //% "cleared more than 50&nbsp;MB of unused data.</p><br />"
        //% "<p>Please consider to make a small donation to support the developer ;)</p>"
        text: qsTrId("mashka-banner")
    }

    Grid {
        id: content
        anchors {
            bottom: parent.bottom
            bottomMargin: Theme.paddingLarge
        }
        width: parent.width
        columns: orientation === Orientation.Portrait ? 1 : 3
        rows: orientation === Orientation.Portrait ? 3 : 1

        PlainButton {
            text: "PayPal"
            onClicked: Qt.openUrlExternally("https://www.paypal.me/osetr")
        }

        PlainButton {
            text: "Yandex.Money"
            onClicked: Qt.openUrlExternally("https://money.yandex.ru/to/410012535782304")
        }

        PlainButton {
            //% "Don't show again"
            text: qsTrId("mashka-not-show")
            onClicked: {
                pageStack.replace(Qt.resolvedUrl("MainPage.qml"))
                mashka.setBannerShowed()
            }
        }
    }
}
