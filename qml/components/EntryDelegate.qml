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
        // This took about 3 hours of my life
        x: 0

        MenuItem {
            id: deleteAllItem
            visible: deleteConfigItem.visible +
                     deleteCacheItem.visible +
                     deleteLocalDataItem.visible > 1
            //% "Delete all data"
            text: qsTrId("mashka-delete-all")
            //% "Deleting all data"
            onClicked: remorseAction(qsTrId("mashka-deleting-alldata"), function() {
                mmodel.deleteData(name, mashka.processConfigEnabled ?
                                     MModel.AllData :
                                     MModel.CacheData | MModel.LocalData)
            })
        }

        MenuItem {
            id: deleteConfigItem
            visible: mashka.processConfigEnabled &&
                     (!installed || mashka.deleteAllDataAllowed) && configSize > 0
            //% "Delete configuration"
            text: qsTrId("mashka-delete-config")
            //% "Deleting configuration"
            onClicked: remorseAction(qsTrId("mashka-deleting-config"), function() {
                mmodel.deleteData(name, MModel.ConfigData)
            })
        }

        MenuItem {
            id: deleteCacheItem
            visible: cacheSize > 0
            //% "Delete cache"
            text: qsTrId("mashka-delete-cache")
            //% "Deleting cache"
            onClicked: remorseAction(qsTrId("mashka-deleting-cache"), function() {
                mmodel.deleteData(name, MModel.CacheData)
            })
        }

        MenuItem {
            id: deleteLocalDataItem
            visible: (!installed || mashka.deleteAllDataAllowed) && localDataSize > 0
            //% "Delete local data"
            text: qsTrId("mashka-delete-localdata")
            //% "Deleting local data"
            onClicked: remorseAction(qsTrId("mashka-deleting-localdata"), function() {
                mmodel.deleteData(name, MModel.LocalData)
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
            visible: mashka.processConfigEnabled
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
