import QtQuick 2.0
import QtQuick.Controls 1.0
import QtQuick.Layouts 1.0
import qml.clientComponents 1.0 as CliComp
import "Call.js" as Call

Rectangle {
    id: rectangle1
    color: "lightgray"
    width: 400
    height: 400

    Component.onCompleted: {
        RPC.setMessageCallback (function (obj, sig, args) {
            Call.output("Message (" + obj + '.' + sig + '):')
            for (var x in args)
                Call.output('[' + JSON.stringify(args [x]) + ']')

            Call.output("\n\n")
        })
    }

    RowLayout {
        id: gl_ctrlPanel

        anchors.left: parent.left
        anchors.leftMargin: 4
        anchors.right: parent.right
        anchors.rightMargin: 4
        anchors.top: parent.top
        anchors.topMargin: 4
        spacing: 0

        TextField {
            id: txtf_object
            text: "file"
            placeholderText: "Object:"
            Layout.maximumWidth: 75
        }
        TextField {
            id: txtf_method
            text: "dir"
            placeholderText: "Method:"
            Layout.maximumWidth: 75
        }
        TextField {
            id: txtf_args
            placeholderText: "args"
            Layout.fillWidth: true
        }
        Button {
            id: btn_callMethod
            text: qsTr("call")

            onClicked: {
                var r_obj = txtf_object.text.trim()
                var r_method = txtf_method.text.trim()
                var args = eval ('[' + txtf_args.text + ']')
                if (r_obj.length == 0 || r_method.length == 0)
                    return;
                var r_call = RPC.newRMethod(r_obj, r_method, args).done (function (rets){
                    for (var x in rets)
                        Call.output ("return" + x + ":" + JSON.stringify(rets[x], null, " ") + '\n');

                    Call.output("\n")
                })
                r_call.call ();
            }
        }
    }

    Rectangle {
        id: rect_output
        color: "black"

        anchors.left: parent.left
        anchors.leftMargin: 4
        anchors.right: parent.right
        anchors.rightMargin: 4
        anchors.bottom: parent.bottom
        anchors.bottomMargin: 4
        anchors.top: gl_ctrlPanel.bottom
        anchors.topMargin: 4

        Flickable {
            id: fli_output
            anchors.rightMargin: 4
            anchors.leftMargin: 4
            anchors.bottomMargin: 4
            anchors.topMargin: 4
            clip: true
            anchors.fill: parent
            contentHeight: txted_output.height

            TextEdit {
                id: txted_output
                color: "#ffffff"
                text: ""
                font.pixelSize: 12
                font.bold: true
                font.family: "Monospace"
                wrapMode: TextEdit.WrapAtWordBoundaryOrAnywhere
                readOnly: true

                anchors.top: parent.top
                anchors.right: parent.right
                anchors.left: parent.left

                onHeightChanged: {
                    var y = txted_output.height - fli_output.height
                    if (y > 0) {
                        fli_output.contentY = y
                    }
                }
            }
        }
    }
}
