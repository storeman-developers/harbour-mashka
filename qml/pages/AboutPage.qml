import QtQuick 2.0
import Sailfish.Silica 1.0


Page {

    SilicaFlickable {
        anchors.fill: parent
        contentHeight: column.height + Theme.paddingMedium

        Column {
            id: column
            width: parent.width
            spacing: Theme.paddingMedium

            PageHeader {
                id: header
                //% "About Mashka"
                title: qsTrId("mashka-about")
            }

            Item {
                height: icon.height + Theme.paddingMedium
                width: parent.width

                Image {
                    id: icon
                    anchors.horizontalCenter: parent.horizontalCenter
                    source: "/usr/share/icons/hicolor/128x128/apps/harbour-mashka.png"
                }
            }

            Label {
                anchors.horizontalCenter: parent.horizontalCenter
                color: Theme.highlightColor
                text: "Mashka " + Qt.application.version
            }

            Label {
                anchors {
                    left: parent.left
                    right: parent.right
                    leftMargin: Theme.horizontalPageMargin
                    rightMargin: Theme.horizontalPageMargin
                }
                height: implicitHeight + Theme.paddingMedium
                color: Theme.highlightColor
                linkColor: Theme.primaryColor
                font.pixelSize: Theme.fontSizeSmall
                wrapMode: Text.WordWrap
                horizontalAlignment: Qt.AlignHCenter
                //% "<p>Mashka is the slang name of the mop for deck washing used by Russian sailors.</p>"
                //% "<p>Mashka searches for and deleted unused data leaved after removed Sailfish&nbsp;OS applications.</p>"
                text: qsTrId("mashka-app-description") + "<br />" +
                      //% "<p>This is an open source software which is distributed "
                      //% "under the terms of the<br /><a href='%1'>MIT License</a>.</p>"
                      qsTrId("mashka-app-licensing").arg("https://github.com/mentaljam/harbour-mashka/blob/master/LICENSE")
                onLinkActivated: Qt.openUrlExternally(link)
            }

            ButtonLayout {
                width: parent.width

                Button {
                    //% "Development"
                    text: qsTrId("mashka-development")
                    onClicked: Qt.openUrlExternally("https://github.com/mentaljam/harbour-mashka")
                }

                Button {
                    //% "Translations"
                    text: qsTrId("mashka-translations")
                    onClicked: Qt.openUrlExternally("https://www.transifex.com/mentaljam/harbour-mashka")
                }

                Button {
                    //% "Donate with %1"
                    text: qsTrId("mashka-donate-with").arg("PayPal")
                    onClicked: Qt.openUrlExternally("https://www.paypal.me/osetr")
                }

                Button {
                    text: qsTrId("mashka-donate-with").arg("Yandex.Money")
                    onClicked: Qt.openUrlExternally("https://money.yandex.ru/to/410012535782304")
                }
            }
        }
    }
}
