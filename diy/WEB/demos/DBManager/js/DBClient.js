var DBMRPC = new RPC ("192.168.1.101", 8000)
var DBMFlow = new flow
DBMFlow.genCall = function () {
    RPC_waiting ()
}
DBMFlow.genReturn = function () {
    RPC_finished ()
}
DBMFlow.bindRPC (DBMRPC)

var DBS = new String

// procs
var p_openDBSession = new Proc (DBMRPC, function () {
    done (this.steps [0], function (rets) {
        DBS = rets [0]
        $('#a_openDB').show()
        $('#a_openDBSession').hide()
        $('#a_exitDBSession').show()
        $('#client_title').text (DBS)
        showDialog ('#openDBDlg')
    })
    callStep (this.steps [0])
}).assignSteps ([
    {t:'j', o:'DBM', m:'DBMSession'}
])

var p_openDatabase = new Proc (DBMRPC, function (db_name, db_pwd) {
    this.steps [0].object = DBS
    done (this.steps [0], function (rets) {
        if (rets [0]) {
            tablesManage ()
            $('#client_title').text (DBS + '-' + db_name)
        } else
            alert("ERROR: Can't Open database.")
    })
    callStep (this.steps [0], db_name, db_pwd)
}).assignSteps ([
    {t:'j', o:'', m:'openDatabase'}
])

var p_listTable = new Proc (DBMRPC, function () {
    this.steps [0].object = DBS
    $('#tablesViewer').html ('')
    done (this.steps [0], function (rets) {
        var tables = rets [0];
        var managerTableHtml = $('<table class="tables"></table>')
        managerTableHtml.append ('<tr><th>Table name</th><th colspan="3">Operator</th></tr>')
        for (var i = 0; i < tables.length; i++) {
            var aTable = tables [i]
            if (aTable.type === "table") {
                var mth_row = $('<tr></tr>')
                managerTableHtml.append (mth_row)
                mth_row.append ($('<td>' + aTable.name + '</td>'))


                var mth_row_alter = $('<td></td>')
                mth_row.append (mth_row_alter)

                var mth_row_alter_a = ($('<a href="javascript:void(0)">Alter</a>'))
                mth_row_alter_a.attr ('data-sql', aTable.sql)
                mth_row_alter_a.attr ('onclick', 'alterTable("' + aTable.name + '", $(this))')

                mth_row_alter.append (mth_row_alter_a)


                mth_row.append ('<td><a href="javascript:dropTable(\'' + aTable.name + '\')">Drop</a></td>')

                mth_row.append ('<td><a href="javascript:renameTable(\'' + aTable.name + '\')">Rename</a></td>')
              }
         }
        managerTableHtml.append ('<tr><td colspan="4"><a href="javascript:addTable()">+Add new table</a></td></tr>')
        $('#tablesViewer').append (managerTableHtml)
    })
    callStep (this.steps [0], "sqlite_master")
}).assignSteps ([
    {t:'j', o:'', m:'DBQueryWithTable'}
])

var p_alterTable = new Proc (DBMRPC, function (aliasFields, tableDef) {
    this.steps [0].object = DBS
    done (this.steps [0], function (rets){
        if (rets [0])
            tablesManage ()
        else {
            alert ("ERROR:" + rets[1])
            returnTablesManage ()
        }
    })
    callStep (this.steps [0], aliasFields, tableDef)
}).assignSteps ([
    {t:'j', o:'', m:'alterTable'}
])

var p_addTable = new Proc (DBMRPC, function (tableDef) {
    this.steps [0].object = DBS
    done (this.steps [0], function (rets) {
        if (rets [0])
            tablesManage ()
        else {
            alert ('ERROR:') + rets[1]
            returnTablesManage ()
        }
    })

    callStep (this.steps [0], tableDef)
}).assignSteps ([
    {t:'j', o:'', m:'createUsrTable'}
])

var p_dropTable = new Proc (DBMRPC, function (tableName) {
    this.steps [0].object = DBS
    done (this.steps [0], function (rets){
        if (rets [0])
            tablesManage ()
        else
            alert ("ERROR: " + rets [1])
    })

    callStep (this.steps [0], tableName)
}).assignSteps ([
    {t:'j', o:'', m:'dropTable'}
])

var p_renameTable = new Proc (DBMRPC, function (tableName, tableNewName) {
    this.steps [0].object = DBS
    done (this.steps [0], function (rets){
        if (rets [0])
            tablesManage ()
        else
            alert ("ERROR: " + rets [1])
    })

    callStep (this.steps [0], tableName, tableNewName)
}).assignSteps ([
    {t:'j', o:'', m:'renameTable'}
])

onConnected (DBMRPC, function () {
    $("#DBMRPC").removeClass ("connected disconnected")
    $("#DBMRPC").addClass ("connected")
    p_openDBSession.call ()
})

onDisconnected (DBMRPC, function () {
    $("#DBMRPC").removeClass ("connected disconnected")
    $("#DBMRPC").addClass ("disconnected")
    $('.dialog').hide()
    $('#a_openDBSession').show()
    $('#a_exitDBSession').hide()
    $('#a_openDB').hide()
    $('#client_title').text('')
})



function openDBSession () {
    if (DBMRPC.hasConnected ())
        p_openDBSession.call ()
    else
        connectToHost (DBMRPC)
}

function openDatabase () {
    var db_name = $('[name="opdb_name"]').val ()
    var db_pwd = $('[name="opdb_pwd"]').val ()
    if (!db_name)
        alert("Input database name")
    else
        p_openDatabase.call (db_name, db_pwd)
}

function tablesManage () {
    showDialog ('#tablesDlg')
    p_listTable.call ()
}
function returnTablesManage () {
    showDialog ('#tablesDlg')
}

function alterTable (table, dataSql) {
    var sql = dataSql.attr ("data-sql")
    sql = sql.replace(/^\s*CREATE TABLE[^\(]*\(/gi,"")
    sql = sql.replace(/\)[^\)]*$/gi,"")

    var fields = sql.split (',')
    var alterCmd = new String
    for (var i in fields)
        alterCmd += fields [i].trim () + '\n'

    $("#tableDefDlg").attr ("data-table", table)
    //$("#tableDefDlg").attr ("data-op", "alter")
    $("#tblDefOKBtn").attr ("onclick", "execAlterTable()")
    $("#tblDefEditor").val (alterCmd)
    $('#tableDefDlg').children ().first().text ('Alter ' + table + ' table')
    showDialog ('#tableDefDlg')
}

function execAlterTable ()
{
    var stms = $("#tblDefEditor").val()
    stms.trim()
    if (stms.length === 0)
        alert ("The table define is empty")
    else {
        var tableDef = new Object
        tableDef.fields = new Array
        tableDef.tableName = $("#tableDefDlg").attr ("data-table")
        var newerFields = new String
        var aliasFields = new String
        var firstField = true

        var lines = stms.split('\n')
        for (var i in lines) {
            var line = lines [i]
            line.trim()

            if (line.length === 0)
                continue

            var nameEnd = line.indexOf (' ')
            if (nameEnd == -1)
                continue

            var namePart = line.substring (0, nameEnd).split (':')
            var oldName = new String
            var name = new String
            if (namePart.length > 1) {
                oldName = namePart [0]
                name = namePart [1]
            } else
                oldName = name = namePart [0]

            tableDef.fields.push(name + ' ' + line.substring(nameEnd + 1))

            if (!firstField)
                aliasFields += ', '
            else
                firstField = false

            if (oldName.length === 0)
                aliasFields += "NULL AS " + name
            else
                aliasFields += oldName + " AS " + name
        }

        p_alterTable.call(aliasFields, tableDef)
    }
}

function addTable () {
    var tableName = prompt("input table name")
    if (tableName != null) {
        if (tableName.length === 0)
            alert("Table name can't be empty")
        else {
            $('#tblDefEditor').val ('')
            $('#tableDefDlg').attr ('data-table', tableName)
            $('#tblDefOKBtn').attr ('onclick', 'execAddTable()')
            $('#tableDefDlg').children ().first().text ('Add ' + tableName + ' table')
            showDialog ("#tableDefDlg")
        }
    }
}

function execAddTable ()
{
    var lines = $('#tblDefEditor').val().split('\n')

    var tableDef = new Object
    tableDef.fields = new Array

    tableDef.tableName = $('#tableDefDlg').attr('data-table')

    for (var i in lines) {
        var theLine = lines [i]
        theLine.trim ()
        if (theLine.length === 0)
            continue

        tableDef.fields.push(theLine)
    }

    p_addTable.call (tableDef)
}

function dropTable (tableName) {
    if (confirm("Do you want to drop the " + tableName + " table?") === true)
        p_dropTable.call (tableName)
}

function renameTable (tableName) {
    var newTableName = prompt("Input the " + tableName + " table new name")
    if (newTableName !== null) {
        if (newTableName.length === 0)
            alert ("Table name can't be empty")
        else
            p_renameTable.call (tableName, newTableName)
    }
}