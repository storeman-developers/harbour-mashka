import QtQuick 2.0
import Sailfish.Silica 1.0
import harbour.mashka 1.0
import "../components"


Page {
    id: page
    allowedOrientations: Orientation.All

    BusyIndicator {
        size: BusyIndicatorSize.Large
        anchors.centerIn: parent
        running: mmodel.busy
    }

    SilicaListView {
        id: listView
        anchors.fill: parent
        visible: !mmodel.busy

        model: MProxyModel {
            id: proxyModel
            sortRole: MModel.SortRole
            sortCaseSensitivity: Qt.CaseInsensitive
            sourceModel: mmodel
        }

        onCountChanged: proxyModel.sort(Qt.AscendingOrder)

        header: PageHeader {
            title: qsTrId("mashka-found")
            description:
                //% "%1 of data"
                qsTrId("mashka-of-data").arg(prettyBytes(
                    mmodel.totalConfigSize + mmodel.totalCacheSize + mmodel.totalLocaldataSize)) + " " +
                //% "of %n application(s)"
                qsTrId("mashka-of-apps", mmodel.totalAppsCount)
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

            RescanMenuEntry { }
        }

        VerticalScrollDecorator { }
    }
}
