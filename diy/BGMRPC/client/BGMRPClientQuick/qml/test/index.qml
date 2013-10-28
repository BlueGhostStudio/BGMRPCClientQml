import QtQuick 2.0
import QtQuick.Controls 1.0

Rectangle {
    id: rect_indexmain
    width: 300
    height: 300
    color: "#00000000"
    property string host: "localhost"
    property int port: 8000
    property bool autoConnect: true

    QtObject {
        id: settings
        property string adminPassword: ""
    }

    Flipable {
        id: fli_cli
//        width: rect_main.width
//        height: rect_main.height
        anchors.fill: parent
        property bool cliPage: false

        front: GridView {
            id: gv_index
            anchors.fill: parent
            cellWidth: 101
            cellHeight: 81
            model: [
                { name: "Call", desc: "Call Rpc method.", qml: "call.qml" },
                { name: "Chat", desc: "Chat Demo.", qml: "chat.qml", opt: 0 },
                { name: "ChatJS", desc: "Chat Demo.\nJsEngine version", qml: "chat.qml", opt: 1 },
                { name: "JsDevelop", desc: "Js run and Dev.", qml: "jsDev.qml" },
                { name: "Settings", desc: "Client Property Setting", qml: "settings.qml" }
            ]
            delegate: Item {
                id: item_indexItem
                height: gv_index.cellHeight
                width: gv_index.cellWidth
                Rectangle {
                    id: rect_indexItem
                    color: "gray"
                    width: item_indexItem.width - 1
                    height: item_indexItem.height - 1
                    x: 1
                    y: 1

                    Text {
                        id: txt_title
                        text: modelData.name

                        anchors.top: parent.top
                        anchors.topMargin: 4
                        anchors.left: parent.left
                        anchors.leftMargin: 4

                        color: "lightgray"
                        font.bold: true
                        font.pointSize: 12
                    }
                    Text {
                        id: txt_desc
                        text: modelData.desc
                        wrapMode: Text.WordWrap
                        verticalAlignment: Text.AlignBottom
                        horizontalAlignment: Text.AlignRight

                        anchors.top: txt_title.bottom
                        anchors.topMargin: 4
                        anchors.bottom: parent.bottom
                        anchors.bottomMargin: 4
                        anchors.left: parent.left
                        anchors.leftMargin: 4
                        anchors.right: parent.right
                        anchors.rightMargin: 4

                        color: "lightgray"
                        font.pointSize: 8
                    }
                    MouseArea {
                        id: ma_indexItem
                        hoverEnabled: true
                        anchors.fill: parent

                        cursorShape: "PointingHandCursor"

                        onClicked: {
                            if (cli_loader.curIndex !== index) {
                                cli_loader.source = ""
                                cli_loader.source = modelData.qml
                                if (cli_loader.item.opt !== undefined)
                                    cli_loader.item.opt = modelData.opt
                                cli_loader.curIndex = index
                            } else
                                fli_cli.cliPage = true
                        }
                    }

                    states: [
                        State {
                            name: "mouseClicked"
                            when: ma_indexItem.pressed
                            PropertyChanges {
                                target: rect_indexItem
                                opacity: 0.5
                            }
                        },
                        State {
                            name: "mouseHover"
                            when: ma_indexItem.containsMouse
                            PropertyChanges {
                                target: txt_title
                                color: "white"
                            }
                            PropertyChanges {
                                target: txt_desc
                                color: "white"
                            }
                        }
                    ]
                }
            }
        }
        back: Rectangle {
            id: rect_cli
            color: "white"
            anchors.fill: parent

            Loader {
                id: cli_loader
                anchors.fill: parent
                property int curIndex: -1

                states: [
                    State {
                        name: "loaded"
                        when: cli_loader.status == Loader.Ready
                        PropertyChanges {
                            target: fli_cli
                            cliPage: true
                        }
                    }

                ]
            }

            Rectangle {
                id: rect_indexBtn
                color: "white"
                anchors.horizontalCenter: parent.horizontalCenter
                anchors.top: parent.bottom

                width: txt_btnIndex.width + 10
                height: txt_btnIndex.height

                Text {
                    id: txt_btnIndex
                    text: "Index"
                    anchors.horizontalCenter: parent.horizontalCenter
                    anchors.verticalCenter: parent.verticalCenter
                }
                MouseArea {
                    id: ma_btnIndex
                    anchors.fill: parent

                    cursorShape: "PointingHandCursor"

                    onClicked: {
                        fli_cli.cliPage = false
                    }
                }
            }
        }

        transform: Rotation {
            id: rotation
            origin.x: fli_cli.width/2
            origin.y: fli_cli.height/2
            axis.x: 0; axis.y: 1; axis.z: 0     // set axis.y to 1 to rotate around y-axis
            angle: 0    // the default angle
        }
        transitions: Transition {
            NumberAnimation { target: rotation; property: "angle"; duration: 500 }
        }

        states: [
            State {
                name: "toCliPage"
                when: fli_cli.cliPage
                PropertyChanges {
                    target: rotation
                    angle: 180

                }
                PropertyChanges {
                    target: gv_index
                    enabled: false
                }
            }
        ]
    }
}
