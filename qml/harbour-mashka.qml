import QtQuick 2.0
import Sailfish.Silica 1.0
import Nemo.Notifications 1.0
import harbour.mashka 1.0

ApplicationWindow {
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
            notification.show(qsTrId("mashka-notification-deleted").arg(Format.formatFileSize(size)))
        }

        onDeletionError: {
            notification.showPopup(
                        //% "An error occured!"
                        qsTrId("mashka-notification-error-title"),
                        //% "Error deleting &quot;%1&quot;."
                        qsTrId("mashka-notification-error-body").arg(Format.formatFileSize(path)),
                        "image://theme/icon-lock-warning")
        }
    }
}
