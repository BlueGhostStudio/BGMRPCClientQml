
import QtQuick 2.0
import QtQuick.Controls 1.0
import QtQuick.Layouts 1.0
import BGMRPC 1.0
import QtQuick.Window 2.1

Rectangle {
    id: mainWin
    width: 400
    height: 400

    RPC {
        id: mainRPC
        host: "microrpc.tk"
        port: 8000

        procs: [
            Proc {
                steps: [
                    LStep {
                        flow: function (mess) {
                            console.log ("in LStep")
                            return mess
                        }
                    },
                    JSStep {
                        object: "chatJs"
                        method: "join"
                    }

                ]

                id: download
                call: function (mess, nick) {
                    steps [0].done (function (rets) {
                        console.log (rets)
                        callStep (steps [1], nick)
                    })
                    steps [1].done (function (rets) {
                        console.log (rets)
                    })
                    callStep (steps [0], mess)
                }
            }
        ]

        onConnected: {
            download.call ("hello world", "micro")
        }
    }
    Component.onCompleted: { mainRPC.connectToHost() }
}
