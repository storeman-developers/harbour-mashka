import QtQuick 2.0
import Sailfish.Silica 1.0
import harbour.mashka 1.0
import "../components"


Page {

    function _updateUi() {
        if (status === PageStatus.Active) {
            if (isPortrait) {
                if (!canNavigateForward) {
                    pageStack.pushAttached(Qt.resolvedUrl("EntriesPage.qml"))
                }
            } else if (canNavigateForward) {
                pageStack.popAttached()
            }
        }
    }

    function _showHints() {
        if (isPortrait && status === PageStatus.Active &&
                mashka.showHint(Mashka.OpenEntriesPage)) {
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

    id: page

    onOrientationTransitionRunningChanged: {
        if (!orientationTransitionRunning) {
            _updateUi()
            _showHints()
        }
    }

    onStatusChanged: {
        _updateUi()
        _showHints()
    }

    BusyIndicator {
        size: BusyIndicatorSize.Large
        anchors.centerIn: parent
        running: mmodel.resetting
    }

    Loader {
        id: loader
        visible: !mmodel.resetting
        anchors.fill: parent
        sourceComponent: isPortrait ? singlePaneComponent : doublePaneComponent
    }

    Component {
        id: singlePaneComponent

        CleanerDashboard {
            anchors.fill: parent
        }
    }

    Component {
        id: doublePaneComponent

        Item {
            anchors.fill: parent

            CleanerDashboard {
                anchors {
                    left: parent.left
                    top: parent.top
                    right: parent.horizontalCenter
                    bottom: parent.bottom
                }
            }

            CleanerEntries {
                anchors {
                    left: parent.horizontalCenter
                    top: parent.top
                    right: parent.right
                    bottom: parent.bottom
                }
            }
        }
    }
}
