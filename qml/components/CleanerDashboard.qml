import QtQuick 2.0
import Sailfish.Silica 1.0
import harbour.mashka 1.0


SilicaFlickable {
    visible: !mmodel.resetting
    contentHeight: header.height + content.height

    PullDownMenu {
        id: menu
        busy: mmodel.busy

        MenuItem {
            text: qsTrId("mashka-about")
            onClicked: pageStack.push(Qt.resolvedUrl("../pages/AboutPage.qml"))
        }

        MenuItem {
            text: qsTrId("mashka-settings")
            onClicked: pageStack.push(Qt.resolvedUrl("../pages/SettingsPage.qml"))
        }

        MenuItem {
            id: deleteUnusedItem
            enabled: mashka.processConfigEnabled ? mmodel.unusedAppsCount > 0 :
                                                   mmodel.unusedCacheSize + mmodel.unusedLocaldataSize > 0

            //% "Delete unused data"
            text: qsTrId("mashka-delete")
            onClicked: pageStack.push(Qt.resolvedUrl("../pages/ConfirmationDialog.qml"))
        }

        RescanMenuEntry { }
    }

    VerticalScrollDecorator { }

    PageHeader {
        id: header
        visible: isPortrait
        height: visible ? _preferredHeight : Theme.paddingLarge
        title: "Mashka"
    }

    Column {
        id: content
        anchors {
            top: header.bottom
            left: parent.left
            leftMargin: Theme.horizontalPageMargin
            right: parent.right
            rightMargin: Theme.horizontalPageMargin
        }
        width: parent.width
        spacing: Theme.paddingMedium

        DataLabel {
            //% "Found"
            title: qsTrId("mashka-found")
            appsCount: mmodel.totalAppsCount
            configSize: mmodel.totalConfigSize
            cacheSize: mmodel.totalCacheSize
            localDataSize: mmodel.totalLocaldataSize
        }

        DataLabel {
            opacity: deleteUnusedItem.enabled ? 1.0 : 0.0
            //% "Unused"
            title: qsTrId("mashka-unused")
            appsCount: mmodel.unusedAppsCount
            configSize: mmodel.unusedConfigSize
            cacheSize: mmodel.unusedCacheSize
            localDataSize: mmodel.unusedLocaldataSize
        }

        Item {
            opacity: mashka.totalDeletedData > 0 ? 1.0 : 0.0
            visible: opacity === 1.0
            width: parent.width
            height: deletedLabel.height + deletedDetailItem.height

            Behavior on opacity { FadeAnimation { } }

            Label {
                id: deletedLabel
                width: parent.width
                horizontalAlignment: Qt.AlignHCenter
                font.pixelSize: Theme.fontSizeLarge
                color: Theme.highlightColor
                wrapMode: Text.WordWrap
                //% "Deleted"
                text: qsTrId("mashka-total-deleted")
            }

            DetailItem {
                id: deletedDetailItem
                anchors.top: deletedLabel.bottom
                label: qsTrId("mashka-total")
                value: Format.formatFileSize(mashka.totalDeletedData)
            }
        }
    }
}
