import QtQuick 2.0
import Sailfish.Silica 1.0
import harbour.mashka 1.0
import "../components"


Page {
    id: page

    onOrientationTransitionRunningChanged: {
        if (!orientationTransitionRunning && isLandscape) {
            pageStack.navigateBack(PageStackAction.Immediate)
        }
    }

    BusyIndicator {
        size: BusyIndicatorSize.Large
        anchors.centerIn: parent
        running: mmodel.resetting
    }

    CleanerEntries {
        anchors.fill: parent
    }
}
