import QtQuick 2.0
import QtWebSockets 1.15

WebSocket {
    property int mID: 0
    property var calling: []

    signal remoteSignal(string obj, string sig, var args)
    signal flowCalling()
    signal flowReturned()

    onTextMessageReceived: {
//        console.log(message)
        var data = JSON.parse(message)

        if (data.type === "signal")
            remoteSignal(data.object, data.signal, data.args)
        else {
            var mID = data.mID

            if (this.calling[mID]) {
                flowReturned()
                this.calling[mID].call(
                         this,
                         data.values.length > 1 ? data.values : data.values[0])
                delete this.calling[mID]
            }
        }
    }

    function callJson(args) {
        var currentCalling = mID.toString()
        var callData = {
            "object": args[0],
            "method": args[1],
            "mID": currentCalling,
            "args": []
        }
        mID++

        for (var x = 2; x < args.length; x++)
            callData.args.push(args[x])

        return [currentCalling, callData]
    }

    function call(remoteObj, roMethod, args) {
        if (status != WebSocket.Open) {
            console.log("no connected")
            return
        }

        var callData = callJson(arguments)

        //        calling = callData[0];
        sendTextMessage(JSON.stringify(callData[1]))

        return {
            "onReturn": (function (cb) {
                this.calling[callData[0]] = cb
            }).bind(this)
        }
    }
    function asyncCall() {
        if (status != WebSocket.Open) {
            console.log("no connected")
            return
        }
        flowCalling()
        var args = arguments
        return new Promise((function (resolve) {
            this.call.apply(this, args).onReturn(function (ret) {
                resolve(ret)
            })
        }).bind(this))
    }
}
