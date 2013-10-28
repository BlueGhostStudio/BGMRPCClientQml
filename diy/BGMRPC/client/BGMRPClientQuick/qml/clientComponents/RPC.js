function connect (host, port, cb) {
    BGMRPC.ONClosed = function () {
        BGMRPC.connectToHost();
    }
    BGMRPC.ONOpened = function () {
        if (cb !== undefined)
            cb ();
        BGMRPC.ONClosed = undefined;
        BGMRPC.ONOpened = undefined;
    }
    BGMRPC.ONError = function () {
        BGMRPC.ONClosed = undefined;
        BGMRPC.ONOpened = undefined;
    }
    if (BGMRPC.isConnected)
        BGMRPC.close();
    else
        BGMRPC.connectToHost(txtf_host.text, txtf_port.text);
}

function isConnected ()
{
    return BGMRPC.isConnected;
}

//function setMessageCallback (obj, method, cb) { BGMRPC.setMessageCallback (obj, method, cb) }

//function setMessageCallback (obj, cb) { BGMRPC.setMessageCallback (obj, cb) }

function setMessageCallback () {
    if (arguments.length == 1)
        BGMRPC.setMessageCallback (arguments [0]);
    else if (arguments.length == 2)
        BGMRPC.setMessageCallback (arguments [0], arguments [1]);
    else if (arguments.length == 3)
        BGMRPC.setMessageCallback (arguments [0], arguments [1], arguments [2]);
}

function newRMethod (obj, method, args)
{
    return BGMRPC.newMethod(obj, method, args);
}

function newLMethod (cb)
{
    return BGMRPC.newLMethod(cb);
}

function newCallGroup (mg)
{
    var callGroup = new Object;

    var start = mg [0];
    var nextMethod = start;
    for (var x in mg) {
        if (x === 0)
            continue;
        nextMethod = nextMethod.next (mg [x]);
    }

    callGroup.group = mg;
    callGroup.call = function () { start.call () }

    return callGroup;
}
