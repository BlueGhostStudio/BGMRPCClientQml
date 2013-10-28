import QtQuick 2.0
import QtQuick.Controls 1.0
import QtQuick.Layouts 1.0

Rectangle {
    id: rectangle1
    width: 300
    height: 300

    GridLayout {
        id: gl_settings
        x: 8
        y: 8
        columns: 2

        Label {
            text: "Admin Password"
            Layout.alignment: Qt.AlignLeft
        }

        TextField {
            id: txtf_adminPassword
            x: 8
            y: 8
            width: 132
            height: 23
            placeholderText: "Admin Passord"
            text: settings.adminPassword
        }
    }

    Button {
        id: btn_ok
        x: 96
        width: 44
        height: 25
        text: "ok"
        anchors.top: gl_settings.bottom
        anchors.topMargin: 41
        anchors.right: gl_settings.right

        onClicked: {
            settings.adminPassword = txtf_adminPassword.text
            fli_cli.cliPage = false
        }
    }
}
