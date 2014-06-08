function flow ()
{
    this.parseLabel = function (args) {
        var label = new String
        for (var i = 0; i < args.length; i++) {
            var val = args [i];
            if (typeof (val) == 'string')
                label += '<span>"' + val + '"</span>'
            else if (typeof (val) == 'boolean')
                label += '<span>' + val.toString() + '</span>'
            else if (Object.prototype.toString.call(val) == '[object Array]')
                label += '<span>[Array]</span>'
            else if (typeof (val) == 'object')
                label += '<span>{Object}</span>'
        }

        return label
    }
    this.appendLogs = function (html) {
        if (this.logs === undefined)
            return;
        var allLogItem = this.logs.children();
        if (this.maxLog !== undefined) {
            if (allLogItem.length >= this.maxLog)
                allLogItem.last().remove()
        }
        this.logs.prepend(html)
    }
    this.genCall = function (step, args) {
        this.appendLogs (
                        '<div class="flow" style="z-index: -1">' +
                            '<div class="active call">CALL</div>' +
                            '<div class="datastream call">' + step.method + '(' + this.parseLabel (args) + ')</div>' +
                            '<div class="server">' + step.Proc.RPC.name + '.<span class="object">' + step.object + '</span></div>' +
                        '</div>')
    }
    this.genReturn = function (step, rets) {
        this.appendLogs (
                        '<div class="flow" style="z-index: -1">' +
                            '<div class="datastream return">[' + this.parseLabel (rets) + ']</div>' +
                            '<div class="active return">RETURN<span class="method">(' + step.method + ')</span></div>' +
                            '<div class="server">' + step.Proc.RPC.name + '.<span class="object">' + step.object + '</span></div>' +
                        '</div>')
    }
    this.genSignal = function (RPC, obj, sig, args) {
        this.appendLogs (
                        '<div class="flow" style="z-index: -1">' +
                            '<div class="datastream signal">' + sig + '[' + this.parseLabel (args) + ']</div>' +
                            '<div class="active signal">SIGNAL</div>' +
                            '<div class="server">' + RPC.name + '.<span class="object">' + obj + '</span></div>' +
                        '</div>')
    }
    this.genConnected = function (RPC) {
        this.appendLogs (
                        '<div class="flow" style="z-index: 0">' +
                            '<div class="active connected">CONNECTED</div>' +
                            '<div class="datastream connected"></div>' +
                            '<div class="server">' +RPC.name + '</div>' +
                        '</div>')
    }
    this.genConnecting = function (RPC) {
        this.appendLogs (
                        '<div class="flow" style="z-index: 0">' +
                            '<div class="active connecting">CONNECTING</div>' +
                            '<div class="datastream connecting"></div>' +
                            '<div class="server">' + RPC.name + '</div>' +
                        '</div>')
    }
    this.genDisConnected = function (RPC) {
        this.appendLogs (
                        '<div class="flow" style="z-index: 0">' +
                            '<div class="active disconnected">DISCONNECTED</div>' +
                            '<div class="datastream disconnected"></div>' +
                            '<div class="server">' + RPC.name + '</div>' +
                        '</div>')
    }
    this.bindRPC = function (rpc) {
        rpc.flow = this
    }
}

function callStep (step) {
    var args = new Array;
    for (var i = 1; i < arguments.length; i++)
       args [i-1] = arguments [i];

    var flow = step.Proc.RPC.flow
    if (flow !== undefined)
        flow.genCall (step, args)

    step.call.apply (step, args)
}

function done (step, onDone) {
    step.done (function (rets){
        var flow = step.Proc.RPC.flow
        if (flow !== undefined)
            flow.genReturn (step, rets)
        if (onDone !== undefined)
            onDone.call (this, rets)
    })
}

function connectToHost (RPC) {
    var flow = RPC.flow
    if (flow !== undefined)
        flow.genConnecting (RPC)
    RPC.connectToHost ()
}

function onMessage (RPC, cb) {
    RPC.onMessage = function (obj, sig, args) {
        var flow = RPC.flow
        if (flow !== undefined)
            flow.genSignal (RPC, obj, sig, args)
        cb (obj,sig, args)
    }
}

function onConnected (RPC, cb) {
    RPC.onConnected = function () {
        var flow = RPC.flow
        if (flow !== undefined)
            flow.genConnected (RPC)
        cb ()
    }
}

function onDisconnected (RPC, cb) {
    RPC.onDisconnected = function () {
        var flow = RPC.flow
        if (flow !== undefined)
            flow.genDisConnected (RPC)
        cb ()
    }
}
