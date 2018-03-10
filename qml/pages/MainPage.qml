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
        running: mmodel.busy
    }

    SilicaFlickable {
        id: flicable
        anchors.fill: parent
        contentHeight: page.height

        PullDownMenu {
            id: menu

            MenuItem {
                text: qsTrId("mashka-about")
                onClicked: pageStack.push(Qt.resolvedUrl("AboutPage.qml"))
            }

            RescanMenuEntry { }
        }

        VerticalScrollDecorator { }

        PageHeader {
            id: header
            title: "Mashka"
        }

        Grid {
            id: content
            visible: !mmodel.busy
            anchors {
                top: header.bottom
                left: parent.left
                leftMargin: Theme.horizontalPageMargin
                right: parent.right
                rightMargin: Theme.horizontalPageMargin
            }
            spacing: Theme.paddingLarge
            columns: page.isPortrait ?
                         1 : (1 + (mashka.totalClearedSize > 0) * 1 + (mmodel.totalUnused > 0) * 1)

            DataLabel {
                //% "Found"
                title: qsTrId("mashka-found")
                //% "%1 of data"
                dataSizeText: qsTrId("mashka-of-data").arg(prettyBytes(mmodel.totalDataSize))
                //% "of %n application(s)"
                appsCountText: qsTrId("mashka-of-apps", mmodel.totalApps)
            }

            DataLabel {
                visible: mmodel.totalUnused > 0
                //% "Unused"
                title: qsTrId("mashka-unused")
                dataSizeText: qsTrId("mashka-of-data").arg(prettyBytes(mmodel.totalUnusedSize))
                //% "of %n removed application(s)"
                appsCountText: qsTrId("mashka-of-removed-apps", mmodel.totalUnused)
            }

            DataLabel {
                visible: mashka.totalClearedSize > 0
                //% "Total cleared"
                title: qsTrId("mashka-total-cleared")
                dataSizeText: qsTrId("mashka-of-data").arg(prettyBytes(mashka.totalClearedSize))
            }
        }

        Loader {
            visible: !mmodel.busy
            anchors {
                horizontalCenter: parent.horizontalCenter
                bottom: parent.bottom
                bottomMargin: Theme.paddingLarge
            }
            sourceComponent: mmodel.totalUnused ? buttonComponent : placeholderComponent
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
