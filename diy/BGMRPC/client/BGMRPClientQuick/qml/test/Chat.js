function output (txt) {
    txted_output.insert(txted_output.length, txt + '<p></p>')
}
function chatMessOutput (txt) {
    output ('<span style="color: green">' + txt + '</span>')
}
function procMessOutput (txt) {
    output ('<span style="color: gray">' + txt + '</span>')
}
function procErrorOutput (txt) {
    output ('<span style="color: red">' + txt + '</span>')
}

function initial (opt) {
    var objName
    if (opt == 0)
        objName = "chat"
    else if (opt == 1)
        objName = "chatJs"
    RPC.setMessageCallback (objName, "joined", function (args){
        chatMessOutput (args [0] + ' has joined.')
    })
    RPC.setMessageCallback (objName, "nicknameChanged", function (args){
        chatMessOutput (args [0] + ' has changed his nick is ' + args [1] + '.')
    })
    RPC.setMessageCallback (objName, "joined", function (args){
        chatMessOutput (args [0] + ' has joined chat.')
    })
    RPC.setMessageCallback (objName, "said", function (args){
        var mess = args[1].replace ("\n", "<br />")
        output ('<b>' + args [0] + '</b> say:<div style="margin-left: 20px">' + mess + '</div>')
    })
    RPC.setMessageCallback (objName, "leaved", function (args){
        chatMessOutput (args[0] + ' has leaved chat.')
    })
}

function joinMethod (nick, cb)
{
    var __join;
    if (rect_chatMain.opt == 0)
        __join = RPC.newRMethod ("chat", "join", [nick])
    else if (rect_chatMain.opt == 1)
        __join = RPC.newRMethod ("chatJs", "js", ["join", nick])

    __join.done (function (rets){
        if (rets [0]) {
            txti_nick.text = rets [1]
            if (cb !== undefined)
                cb.call ()
        }
    })

    return __join
}

function say () {
    var nick = txti_nick.text
    var mess = txte_say.text

    nick.trim()
    mess.trim()
    if (nick.length == 0 || mess.length == 0)
        return;

    var __say, __join
    if (rect_chatMain.opt == 0)
        __say = RPC.newRMethod ("chat", "say", [mess])
    else if (rect_chatMain.opt == 1)
        __say = RPC.newRMethod ("chatJs", "js", ["say", mess])

    __say.done (function (rets){
        if (rets[0]) {
            txte_say.text = ""
            return false;
        }
    })
    __join = joinMethod(nick, __say)

    __say.next (__join)
    __say.call ()
}

function changeNickname () {
    var nick = txti_nick.text
    nick.trim ()
    if (nick.length == 0)
        return

    var __changeNickname, __join
    if (rect_chatMain.opt == 0)
        __changeNickname = RPC.newRMethod ("chat", "changeNickname", [nick])
    else if (rect_chatMain.opt == 1)
        __changeNickname = RPC.newRMethod ("chatJs", "js", ["changeNickname", nick])

    __changeNickname.done (function (rets){
        if (rets[0])
            return false;
    })
    __join = joinMethod(nick, undefined)

    __changeNickname.next (__join)
    __changeNickname.call ()
}

function whoList () {
    var __whoList
    if (rect_chatMain.opt == 0)
        __whoList = RPC.newRMethod ("chat", "whoList", [])
    else if (rect_chatMain.opt == 1)
        __whoList = RPC.newRMethod ("chatJs", "js", ["whoList"])

    __whoList.done (function (rets) {
        console.log(JSON.stringify(rets))
        if (rets[0]) {
            for (var x in rets [1])
                chatMessOutput (rets [1][x])
        }
    })

    __whoList.call ()
}

function leave () {
    if (rect_chatMain.opt == 0)
        RPC.newRMethod ("chat", "leave", []).call ()
    else if (rect_chatMain.opt == 1)
        RPC.newRMethod ("chatJs", "js", ["leave"]).call ()
}
