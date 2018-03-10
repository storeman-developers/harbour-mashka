import QtQuick 2.0
import Sailfish.Silica 1.0
import Nemo.Notifications 1.0
import harbour.mashka 1.0

ApplicationWindow {

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
            //% "%n KB"
            return qsTrId("mashka-kb", bytes)
        case 2:
            //% "%n MB"
            return qsTrId("mashka-mb", bytes)
        case 3:
            //% "%n GB"
            return qsTrId("mashka-gb", bytes)
        default:
            return ""
        }
    }

    initialPage: mashka.showBanner() ? Qt.resolvedUrl("pages/BannerPage.qml") :
                                      Qt.resolvedUrl("pages/MainPage.qml")
    cover: Qt.resolvedUrl("cover/CoverPage.qml")
    allowedOrientations: defaultAllowedOrientations

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

        onCleared: {
            mashka.addClearedSize(size)
            //% "Cleared %1"
            notification.show(qsTrId("mashka-notification-cleared").arg(prettyBytes(size)))
        }

        onError: {
            notification.showPopup(
                        //% "An error occured!"
                        qsTrId("mashka-notification-error-title"),
                        //% "Error removing &quot;%1&quot;."
                        qsTrId("mashka-notification-error-body").arg(prettyBytes(path)),
                        "image://theme/icon-lock-warning")
        }
    }
}
