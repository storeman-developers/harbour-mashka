import QtQuick 2.0
import Sailfish.Silica 1.0
import harbour.mashka 1.0
import "../components"


Page {
    id: page
    allowedOrientations: Orientation.All

    onStatusChanged: {
        if (status === PageStatus.Active &&
            !pageStack._currentContainer.attachedContainer) {
            pageStack.pushAttached(Qt.resolvedUrl("EntriesPage.qml"))
        }
    }

    Component.onCompleted: {
        if (mashka.showHint(Mashka.OpenEntriesPage)) {
            var hintComp = Qt.createComponent(Qt.resolvedUrl("../components/Hint.qml"))
            var hintObj = hintComp.createObject(page, {direction: TouchInteraction.Left})

            var labelComp = Qt.createComponent(Qt.resolvedUrl("../components/HintLabel.qml"))
            var labelObj = labelComp.createObject(page, {
                                                      hint: hintObj,
                                                      //% "Swipe to see details"
                                                      text: qsTrId("mashka-hint-openentriespage"),
                                                      "anchors.bottom": page.bottom
                                                  })

            labelObj.finished.connect(function() {
                mashka.setHintShowed(Mashka.OpenEntriesPage)
                labelObj.destroy()
                hintObj.destroy()
            })

            hintObj.start()
        }
    }

    BusyIndicator {
        size: BusyIndicatorSize.Large
        anchors.centerIn: parent
        running: mmodel.resetting
    }

    SilicaFlickable {
        id: flicable
        visible: !mmodel.resetting
        anchors.fill: parent
        contentHeight: Math.max(
            page.height,
            header.height + grid.height + clearedItem.height + buttonLoader.height)

        PullDownMenu {
            id: menu
            busy: mmodel.busy

            MenuItem {
                text: qsTrId("mashka-about")
                onClicked: pageStack.push(Qt.resolvedUrl("AboutPage.qml"))
            }

            MenuItem {
                text: qsTrId("mashka-settings")
                onClicked: pageStack.push(Qt.resolvedUrl("SettingsPage.qml"))
            }

            RescanMenuEntry { }
        }

        VerticalScrollDecorator { }

        PageHeader {
            id: header
            visible: orientation === Orientation.Portrait
            height: visible ? _preferredHeight : Theme.paddingLarge
            title: "Mashka"
        }

        Grid {
            id: grid
            anchors {
                top: header.bottom
                left: parent.left
                leftMargin: Theme.horizontalPageMargin
                right: parent.right
                rightMargin: Theme.horizontalPageMargin
            }
            spacing: Theme.paddingMedium
            columns: page.isPortrait ? 1 : (1 + (mmodel.unusedAppsCount > 0) * 1)

            DataLabel {
                //% "Found"
                title: qsTrId("mashka-found")
                appsCount: mmodel.totalAppsCount
                configSize: mmodel.totalConfigSize
                cacheSize: mmodel.totalCacheSize
                localDataSize: mmodel.totalLocaldataSize
            }

            DataLabel {
                //% "Unused"
                title: qsTrId("mashka-unused")
                appsCount: mmodel.unusedAppsCount
                configSize: mmodel.unusedConfigSize
                cacheSize: mmodel.unusedCacheSize
                localDataSize: mmodel.unusedLocaldataSize
            }
        }

        Item {
            id: clearedItem
            anchors {
                top: grid.bottom
                topMargin: Theme.paddingMedium
            }
            opacity: mashka.totalClearedSize > 0 ? 1.0 : 0.0
            visible: opacity === 1.0
            width: parent.width
            height: clearedLabel.height + clearedDetailItem.height

            Behavior on opacity { FadeAnimation { } }

            Label {
                id: clearedLabel
                width: parent.width
                horizontalAlignment: Qt.AlignHCenter
                font.pixelSize: Theme.fontSizeLarge
                color: Theme.highlightColor
                wrapMode: Text.WordWrap
                //% "Cleared"
                text: qsTrId("mashka-total-cleared")
            }

            DetailItem {
                id: clearedDetailItem
                anchors.top: clearedLabel.bottom
                label: qsTrId("mashka-total")
                value: prettyBytes(mashka.totalClearedSize)
            }
        }

        Loader {
            id: buttonLoader
            anchors {
                horizontalCenter: parent.horizontalCenter
                bottom: parent.bottom
                bottomMargin: Theme.paddingLarge
            }
            sourceComponent: mmodel.unusedAppsCount ? buttonComponent : placeholderComponent
        }
    }

    Component {
        id: buttonComponent

        Button {
            //% "Clear unused data"
            text: qsTrId("mashka-clear")
            onClicked: pageStack.push(Qt.resolvedUrl("ConfirmationDialog.qml"))
        }
    }

    Component {
        id: placeholderComponent

        Label {
            //% "No unused data was found"
            text: qsTrId("mashka-placeholder")
            wrapMode: Text.WordWrap
        }
    }
}
