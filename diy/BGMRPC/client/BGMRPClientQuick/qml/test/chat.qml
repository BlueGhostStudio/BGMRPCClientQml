import QtQuick 2.0
import QtQuick.Layouts 1.0
import qml.clientComponents 1.0 as CliComps
import "Chat.js" as Chat

Rectangle {
    id: rect_chatMain
    width: 300
    height: 300

    property int opt: 0

    Component.onDestruction: {
        Chat.leave()
    }

    states: [
        State {
            name: "chat"
            when: rect_chatMain.opt == 0
            StateChangeScript {
                script: { Chat.initial(0) }
            }
        },

        State {
            name: "jsChat"
            when: rect_chatMain.opt == 1
            StateChangeScript {
                script: { Chat.initial(1) }
            }
        }
    ]

    Flickable {
        id: fli_chatOutput
        clip: true
        flickableDirection: Flickable.VerticalFlick
        anchors.right: parent.right
        anchors.rightMargin: 8
        anchors.left: parent.left
        anchors.leftMargin: 8
        anchors.bottom: rw_ctrl.top
        anchors.bottomMargin: 8
        anchors.top: parent.top
        anchors.topMargin: 8

        contentHeight: txted_output.height

        TextEdit {
            id: txted_output
            text: "<p> </p>"
            textFormat: TextEdit.RichText
            wrapMode: TextEdit.WordWrap
            readOnly: true

            anchors.top: parent.top
            anchors.left: parent.left
            anchors.right: parent.right

            onHeightChanged: {
                var y = txted_output.height - fli_chatOutput.height
                if (y > 0)
                    fli_output.contentY = y
            }
        }

    }

    ColumnLayout {
        id: rw_ctrl

        spacing: 2

        anchors.left: parent.left
        anchors.leftMargin: 8
        anchors.right: parent.right
        anchors.rightMargin: 8
        anchors.bottom: parent.bottom
        anchors.bottomMargin: 8

        property int textHeight: 16

        RowLayout {
            spacing: 2
            Text {
                text: "Nick:"
                font.bold: true
                font.pixelSize: rw_ctrl.textHeight
            }

            TextInput {
                id: txti_nick
                text: "noname"
                font.pixelSize: rw_ctrl.textHeight

                Layout.fillWidth: true

                Keys.onPressed: {
                    if (event.key === Qt.Key_Return) {
                        Chat.changeNickname();
                    }
                }
            }
        }

        RowLayout {
            spacing: 2
            Text {
                text: "Say:"
                font.bold: true
                font.pixelSize: rw_ctrl.textHeight

                Layout.alignment: Qt.AlignTop | Qt.AlignRight
            }

            TextEdit {
                id: txte_say
                text: ""
                wrapMode: TextEdit.WordWrap
                font.pixelSize: 0

                Layout.fillWidth: true

                Keys.onPressed: {
                    if (event.key === Qt.Key_Return
                            && (event.modifiers & Qt.ControlModifier)) {
                        Chat.say();
                    }
                }
            }
        }

        RowLayout {
            spacing: 2
            CliComps.Button {
                id: btn_loadChat
                label: "load chat plugin"
                size: rw_ctrl.textHeight

                onClicked: AdminTools.loadObject(settings.adminPassword, 'chat', 'libchatPlugin.so', 'chat',
                                                 function (title, decision) {
                                                     Chat.procMessOutput(title)

                                                     if (decision)
                                                         Chat.procMessOutput("ok")
                                                     else
                                                         Chat.procErrorOutput("fail")
                                                 })
            }

            CliComps.Button {
                id: btn_wholist
                label: "WhoList"
                size: rw_ctrl.textHeight

                onClicked: Chat.whoList()
            }

            CliComps.Button {
                id: btn_leave
                label: "Leave"
                size: rw_ctrl.textHeight

                onClicked: Chat.leave()
            }
        }
    }
}
