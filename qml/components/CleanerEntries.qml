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
        sourceModel: mmodel
    }

    onCountChanged: proxyModel.sort(Qt.AscendingOrder)

    header: PageHeader {
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
