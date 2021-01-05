import QtQuick 2.0
import Sailfish.Silica 1.0
import harbour.mashka 1.0


SilicaListView {
    id: listView
    visible: !mmodel.resetting

    model: MProxyModel {
        id: proxyModel
        sortRole: MModel.SortRole
        sortCaseSensitivity: Qt.CaseInsensitive
        filterCaseSensitivity: Qt.CaseInsensitive
        sourceModel: mmodel
        processConfig: mashka.processConfigEnabled
        onSourceModelChanged: sort(Qt.AscendingOrder)
    }

    header: Item {
        property real _prevHeight: 0

        width: parent.width
        height: pageHeader.height + searchField.height + Theme.paddingSmall

        onHeightChanged: {
            if (height > _prevHeight) {
                _prevHeight = height
                positionViewAtBeginning()
            }
        }

        PageHeader {
            id: pageHeader
            visible: isPortrait
            height: visible ? _preferredHeight : 0.0
            title: qsTrId("mashka-found")
            description:
                //% "%1 of data"
                qsTrId("mashka-of-data").arg(prettyBytes(
                    mmodel.totalConfigSize + mmodel.totalCacheSize + mmodel.totalLocaldataSize)) + " " +
                //% "of %n application(s)"
                qsTrId("mashka-of-apps", mmodel.totalAppsCount)
        }

        SearchField {
            id: searchField
            width: parent.width
            anchors {
                top: pageHeader.bottom
                topMargin: Theme.paddingSmall
            }
            onTextChanged: searchDelay.restart()

            Timer {
                id: searchDelay
                interval: 500
                repeat: false
                onTriggered: proxyModel.setFilterFixedString(searchField.text)
            }
        }
    }

    section {
        property: "installed"
        delegate: SectionHeader {
            text: section === "true" ?
                      //% "Installed"
                      qsTrId("mashka-installed") :
                      //% "Uninstalled"
                      qsTrId("mashka-uninstalled")
        }
    }

    delegate: EntryDelegate { }

    PullDownMenu {
        id: menu
        visible: isPortrait
        busy: mmodel.busy

        RescanMenuEntry { }
    }

    VerticalScrollDecorator { }
}
