
import QtQuick 2.0
import QtQuick.Controls 1.0
import QtQuick.Layouts 1.0
import qml.clientComponents 1.0 as CliComps


Rectangle {
    id: rect_main
    width: 360
    height: 360

    Component.onCompleted: {
        cli_loader.source = cliQml;
    }

    Rectangle {
        id: rect_statusBar
        y: 327
        height: 20
        color: "#000000"
        opacity: 1

        anchors.left: parent.left
        anchors.leftMargin: 0
        anchors.right: parent.right
        anchors.rightMargin: 0
        anchors.bottom: parent.bottom
        anchors.bottomMargin: 0

        CliComps.Button {
            id: btn_back
            label: "< Return"
            color: "white"
            size: 18
            opacity: 0
            enabled: false
            padding: 4

            anchors.left: parent.left
            anchors.leftMargin: 2
            anchors.verticalCenter: parent.verticalCenter

            onClicked: rect_main.state=""
        }

        Text {
            id: txt_status
            color: "white"
            font.bold: true
            horizontalAlignment: Text.AlignRight
            anchors.right: parent.right
            anchors.rightMargin: 4
            anchors.verticalCenter: parent.verticalCenter
            font.pixelSize: 12
        }

        states: [
            State {
                name: "connected"
                when: RPC.isConnected ()
                PropertyChanges {
                    target: txt_status
                    text: qsTr("connected")
                }
            },
            State {
                name: "disconnected"
                when: !RPC.isConnected ()
                PropertyChanges {
                    target: txt_status
                    text: qsTr("disconnected")
                }
            }
        ]
    }

    Rectangle {
        id: rect_home

        color: "#00000000"

        width: rect_main.width
        height: rect_main.height - rect_statusBar.height
        x: 0
        y: 0

        GridLayout {
            id: grid_layout1
            x: 102
            y: 119
            width: 157
            height: 71
            anchors.verticalCenterOffset: -25
            anchors.verticalCenter: parent.verticalCenter
            anchors.horizontalCenterOffset: 1
            anchors.horizontalCenter: parent.horizontalCenter
            rowSpacing: 0
            columnSpacing: 0
            rows: 3
            columns: 2

            Label {
                id: lab_host
                text: "Host:"
            }

            TextField {
                id: txtf_host
                Layout.fillWidth: true
                placeholderText: "Text Field"
            }

            Label {
                id: lab_port
                text: "Port:"
            }

            TextField {
                id: txtf_port
                Layout.fillWidth: true
                placeholderText: "Text Field"
            }

            Button {
                id: btn_connect
                text: "Connect"
                anchors.top: txtf_port.bottom
                anchors.topMargin: 10
                anchors.right: parent.right
                anchors.rightMargin: 0
                Layout.columnSpan: 2

                onClicked: {
                    RPC.connect (txtf_host.text,
                                      txtf_port.text,
                                      function () {
                                          rect_main.state = "toCliPage"
                                      })
                }
            }
        }

    }

    Rectangle {
        id: rect_cli
        color: "#00000000"

        width: rect_main.width
        height: rect_main.height - rect_statusBar.height
        x: rect_main.width
        y: 0

        Loader {
            id: cli_loader
            anchors.fill: parent
            Text {
                id: loader_status
                text: qsTr("text")
                wrapMode: Text.WrapAtWordBoundaryOrAnywhere
                font.pointSize: 13
                font.bold: true
                anchors.fill: parent
                verticalAlignment: Text.AlignVCenter
                horizontalAlignment: Text.AlignHCenter
            }

            states: [
                State {
                    name: "loaded"
                    when: cli_loader.status == Loader.Ready
                    StateChangeScript {
                        script: {
                            var host = cli_loader.item.host;
                            var port = cli_loader.item.port;

                            if (host === undefined)
                                host = "localhost"
                            if (port === undefined)
                                port = 8000

                            txtf_host.text = host;
                            txtf_port.text = port;

                            if (cli_loader.item.autoConnect) {
                                ani_mainTransAni.enabled = false;
                                rect_main.state = "toCliPage";
                                ani_mainTransAni.enabled = true;
                                RPC.connect (host, port);
                            }
                        }
                    }

                    PropertyChanges {
                        target: loader_status
                        visible: false
                    }
                },
                State {
                    name: "error"
                    when: cli_loader.status == Loader.Error
                    PropertyChanges {
                        target: loader_status
                        text: qsTr ("Can't load " + cli_loader.source)
                    }
                }

            ]
        }
    }

    states: [
        State {
            name: "toCliPage"
            PropertyChanges {
                target: rect_home
                x: -rect_main.width
            }
            PropertyChanges {
                target: rect_cli
                x: 0
            }
            PropertyChanges {
                target: btn_back
                opacity: 1
                enabled: true
            }
        }

    ]

    transitions: Transition {
        id: ani_mainTransAni
        NumberAnimation {
            id: ani_home
            target: rect_home
            easing.type: Easing.InOutBack
            property: "x"
            duration: 250
        }
        NumberAnimation {
            target: rect_cli
            easing.type: ani_home.easing.type
            property: "x"
            duration: ani_home.duration
        }
        NumberAnimation {
            target: btn_back
            easing.type: ani_home.easing.type
            property: "opacity"
            duration: ani_home.duration
        }
    }
}
