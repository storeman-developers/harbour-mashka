import QtQuick 2.0
import Sailfish.Silica 1.0
import Nemo.Notifications 1.0
import harbour.mashka 1.0

ApplicationWindow {
    readonly property var _locale: Qt.locale()

    function prettyBytes(bytes) {
        var i = 0
        while (bytes > 1024 && i < 4) {
            bytes /= 1024
            i += 1
        }

        switch (i) {
        case 0:
            //% "%n byte(s)"
            return qsTrId("mashka-bytes", bytes)
        case 1:
            //% "%1 KB"
            return qsTrId("mashka-kb").arg(Number(bytes).toLocaleString(_locale, 'f', 0))
        case 2:
            //% "%1 MB"
            return qsTrId("mashka-mb").arg(Number(bytes).toLocaleString(_locale, 'f', 0))
        case 3:
            //% "%1 GB"
            return qsTrId("mashka-gb").arg(Number(bytes).toLocaleString(_locale, 'f', 1))
        default:
            return ""
        }
    }

    initialPage: mashka.showBanner() ? Qt.resolvedUrl("pages/BannerPage.qml") :
                                       Qt.resolvedUrl("pages/MainPage.qml")
    cover: Qt.resolvedUrl("cover/CoverPage.qml")
    _defaultPageOrientations: Orientation.All

    Notification {

        function show(text, icn) {
            replacesId = 0
            previewSummary = ""
            previewBody = text
            icon = icn ? icn : ""
            publish()
        }

        function showPopup(title, text, icn) {
            replacesId = 0
            previewSummary = title
            previewBody = text
            icon = icn
            publish()
        }

        id: notification
        expireTimeout: 3000
    }

    Mashka {
        id: mashka
    }

    MModel {
        id: mmodel

        onDataDeleted:  {
            mashka.addDeletedData(size)
            //% "Deleted %1"
            notification.show(qsTrId("mashka-notification-deleted").arg(prettyBytes(size)))
        }

        onDeletionError: {
            notification.showPopup(
                        //% "An error occured!"
                        qsTrId("mashka-notification-error-title"),
                        //% "Error deleting &quot;%1&quot;."
                        qsTrId("mashka-notification-error-body").arg(prettyBytes(path)),
                        "image://theme/icon-lock-warning")
        }
    }
}
