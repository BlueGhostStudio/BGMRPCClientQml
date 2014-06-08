function RPC (host,port)
{
    this.host = host
    this.port = port
    this.callingSteps = new Object
    this.mID = 0
    this.ws = undefined

    this.hasConnected = function () {
        if (this.ws === undefined || this.ws.readyState !== 1)
            return false;
        else
            return true;
    } 
    this.reviceData = function (evt) {
        var data = JSON.parse (evt.data)
        if (data.type === "signal") {
            if (this.RPC.onMessage !== undefined)
                this.RPC.onMessage (data.object, data.signal, data.args)
        } else if (data.type === "return") {
            var mID = data.mID
            var step = this.RPC.callingSteps [mID]
            var onDone = step.onDone
            if (onDone !== undefined) {
                onDone.call (step.Proc, data.values)
            }

            delete this.RPC.callingSteps [mID]
        }
    }
    this.connectToHost = function () {
        this.ws = new WebSocket("ws://" + host + ":" + port)
        this.ws.RPC = this
        this.ws.onopen = this.onConnected
        this.ws.onclose = this.onDisconnected
        this.ws.onerror = this.onError
        this.ws.onmessage = this.reviceData
    }
    this.close = function () {
        this.ws.close ()
    }

    this.call = function (step, object, method, args) {
        if (this.ws.readyState !== 1)
            return

        var mID = object + '_' + method + this.mID
        this.mID++
        this.callingSteps [mID] = step
        
        var callJson = new Object
        callJson.object = object
        callJson.method = method
        callJson.args = args
        callJson.mID = mID

        this.ws.send (JSON.stringify(callJson));
    }
}

function RStep (Proc, object, method)
{
    this.Proc = Proc
    this.object = object
    this.method = method
    this.done = function (dcb) {
        this.onDone = dcb
        return this
    }
    
    this.call = function () {
        var args = new Array
        for (var i = 0; i < arguments.length; i++)
            args [i] = arguments [i]

        this.Proc.RPC.call (this, this.object, this.method, args)
    }
}

function JSStep (Proc, object, method)
{
    this.Proc = Proc
    this.object = object
    this.method = method
    this.done = function (dcb) {
        this.onDone = dcb
        return this
    }
    
    this.call = function () {
        var args = new Array
        args [0] = this.method
        for (var i = 0; i < arguments.length; i++)
            args [i+1] = arguments [i]

        this.Proc.RPC.call (this, this.object, "js", args)
    }
}

function LStep (Proc, flow) {
    this.Proc = Proc
    this.onFlow = flow
    this.done = function (dcb) {
        this.onDone = dcb
        return this
    }
    
    this.call = function () {
        if (this.onFlow !== undefined)
            this.onDone.call (this.Proc, this.onFlow (arguments))
    }
}

function Proc (RPC, call) {
    this.RPC = RPC
    this.call = call
    this.steps  = new Array
    this.assignSteps = function (steps) {
        this.steps = new Array
        for (var i = 0; i < steps.length; i++) {
            var type = steps [i].t
            if (type === 'r')
                this.steps [i] = new RStep (this, steps [i].o, steps [i].m)
            else if (type === 'j')
                this.steps [i] = new JSStep (this, steps [i].o, steps[i].m)
            else if (type === 'l')
                this.steps [i] = new LStep (this, steps [i].f)
        }

        return this
    }
}
