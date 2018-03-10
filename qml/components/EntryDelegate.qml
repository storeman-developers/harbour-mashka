import QtQuick 2.0
import Sailfish.Silica 1.0
import harbour.mashka 1.0


ListItem {
    id: delegate
    width: parent.width
    contentHeight: content.height + Theme.paddingMedium * 2
    showMenuOnPressAndHold: deleteAllItem.visible || deleteConfigItem.visible ||
                            deleteCacheItem.visible || deleteLocalDataItem.visible

    menu: ContextMenu {

        MenuItem {
            id: deleteAllItem
            visible: deleteConfigItem.visible +
                     deleteCacheItem.visible +
                     deleteLocalDataItem.visible > 1
            //% "Delete all data"
            text: qsTrId("mashka-delete-all")
            //% "Deleting all data"
            onClicked: Remorse.popupAction(page, qsTrId("mashka-deleting-alldata"), function() {
                mmodel.clearData(name, MModel.AllData)
            })
        }

        MenuItem {
            id: deleteConfigItem
            visible: !installed && configSize > 0
            //% "Delete configuration"
            text: qsTrId("mashka-delete-config")
            //% "Deleting configuration"
            onClicked: Remorse.popupAction(page, qsTrId("mashka-deleting-config"), function() {
                mmodel.clearData(name, MModel.ConfigData)
            })
        }

        MenuItem {
            id: deleteCacheItem
            visible: cacheSize > 0
            //% "Delete cache"
            text: qsTrId("mashka-delete-cache")
            //% "Deleting cache"
            onClicked: Remorse.popupAction(page, qsTrId("mashka-deleting-cache"), function() {
                mmodel.clearData(name, MModel.CacheData)
            })
        }

        MenuItem {
            id: deleteLocalDataItem
            visible: !installed && localDataSize > 0
            //% "Delete local data"
            text: qsTrId("mashka-delete-localdata")
            //% "Deleting local data"
            onClicked: Remorse.popupAction(page, qsTrId("mashka-deleting-localdata"), function() {
                mmodel.clearData(name, MModel.LocalData)
            })
        }
    }

    Image {
        id: iconItem
        anchors {
            verticalCenter: parent.verticalCenter
            left: parent.left
            leftMargin: Theme.horizontalPageMargin
        }
        visible: icon
        source: icon
    }

    Column {
        id: content
        anchors {
            verticalCenter: parent.verticalCenter
            left: iconItem.visible ? iconItem.right : parent.left
            leftMargin: Theme.horizontalPageMargin
            right: parent.right
            rightMargin: Theme.horizontalPageMargin
        }

        Label {
            width: parent.width
            wrapMode: Text.WordWrap
            text: title
            color: delegate.highlighted ? Theme.highlightColor : Theme.primaryColor
        }

        SizeLabel {
            //% "Configuration"
            label: qsTrId("mashka-config")
            value: configSize
        }

        SizeLabel {
            //% "Cache"
            label: qsTrId("mashka-cache")
            value: cacheSize
        }

        SizeLabel {
            //% "Local data"
            label: qsTrId("mashka-localdata")
            value: localDataSize
        }
    }
}
