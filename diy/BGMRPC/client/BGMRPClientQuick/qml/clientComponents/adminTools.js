function loadObject (admin_pwd, plugin, pluginFile, obj, logCallback) {
    console.log(admin_pwd)
    var admin_login, check_plugin, load_plugin, check_obj, create_obj

    admin_login = RPC.newRMethod ("ctrl", "login", [admin_pwd]).done (function (rets) {
        if (!rets[0]) {
            logCallback ("Login...", false)
            return false;
        }

        logCallback ("login...", true)
    })
    check_plugin = RPC.newRMethod ("ctrl", "RPCTypes", []).done (function (rets){
        var logTitle = 'check ' + plugin + ' plugin...'
        for (var i in rets[0]) {
            if (rets[0][i] === plugin) {
                logCallback (logTitle, true)
                check_obj.call ()
                return false
            }
        }
        logCallback (logTitle, false)
    })
    load_plugin = RPC.newRMethod ("ctrl", "loadPlugin", [pluginFile]).done (function (rets) {
        var logTitle = 'Load ' + pluginFile + '...'
        if (!rets[0]) {
            logCallback (logTitle, false)
            return false
        }

        logCallback (logTitle, true)
    })
    check_obj = RPC.newRMethod ("ctrl", "RPCObjects", []).done (function (rets){
        var logTitle = 'Check ' + obj + '...'
        for (var i in rets[0]) {
            if (rets[0][i].name === obj) {
                logCallback (logTitle, true)
                return false
            }
        }

        logCallback (logTitle, false)
    })
    create_obj = RPC.newRMethod ("ctrl", "RPCCreateObject", [plugin, obj]).done (function (rets) {
        var logTitle = 'Creator ' + obj + '...'
        if (rets[0])
            logCallback (logTitle, true)
        else
            logCallback (logTitle, false)
    })

    RPC.newCallGroup ([admin_login, check_plugin, load_plugin, check_obj, create_obj]).call ()
}
