import QtQuick 2.0
import Sailfish.Silica 1.0
import "../components"


Page {
    allowedOrientations: Orientation.All

    SilicaFlickable {
        anchors.fill: parent
        contentHeight: column.height + Theme.paddingMedium

        Column {
            id: column
            width: parent.width

            PageHeader {
                id: header
                //% "Settings"
                title: qsTrId("mashka-settings")
            }

            SectionHeader {
                //% "Counters"
                text: qsTrId("mashka-counters")
            }

            ListMenuItem {
                enabled: mashka.totalClearedSize > 0
                iconSource: "image://theme/icon-m-delete"
                //% "Reset the counter of deleted data"
                text: qsTrId("mashka-reset-counter")
                //% "Resetting"
                onClicked: remorseAction(qsTrId("mashka-resetting"), function() {
                    mashka.resetClearedSize()
                })
            }

            SectionHeader {
                //: A section name of the settings page
                //% "Advanced"
                text: qsTrId("mashka-advanced")
            }

            TextSwitch {
                id: advancedSwitch
                checked: mashka.advancedOptionsEnabled
                //% "Enable advanced options"
                text: qsTrId("mashka-show-enabled")

                onCheckedChanged: mashka.advancedOptionsEnabled = checked
            }

            TextSwitch {
                id: allowDeleteSwitch
                visible: opacity > 0.0
                opacity: advancedSwitch.checked ? 1.0 : 0.0
                checked: mashka.deleteAllDataAllowed
                //% "Allow delete all data for installed applications"
                text: qsTrId("mashka-allow-delete-all")
                //% "By default only cache can be deleted"
                description: qsTrId("mashka-allow-delete-all-note")

                onCheckedChanged: mashka.deleteAllDataAllowed = checked

                Behavior on opacity { FadeAnimation { } }

                // A workaround as not all the signals are emitted
                Connections {
                    target: mashka
                    onDeleteAllDataAllowedChanged: {
                        if (allowDeleteSwitch.checked !== mashka.deleteAllDataAllowed) {
                            allowDeleteSwitch.checked = mashka.deleteAllDataAllowed
                        }
                    }
                }
            }
        }
    }
}
