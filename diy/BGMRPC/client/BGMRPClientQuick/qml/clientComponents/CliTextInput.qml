import QtQuick 2.0

Rectangle {
    id: rect_input
    width: 200
    height: 24

    property string label: "Label:"
    property int labelWidth: -1
    property int echoMode: TextInput.Normal
    property string text
    property color labelTextColor
    property color inputBG: "white"
    property color inputTextColor
    property int inputTextFontSize: 16
    property bool readOnly: false

    color: "#999"

//    FocusScope {
//        anchors.fill: parent

        Text {
            id: txt_label
            text: label
            color: labelTextColor
            horizontalAlignment: Text.AlignRight
            verticalAlignment: Text.AlignVCenter
            width: { labelWidth > 0 ? labelWidth : contentWidth }
            fontSizeMode: Text.Fit
            font.pixelSize: parent.height - 10

            anchors.left: parent.left
            anchors.leftMargin: 2
            anchors.verticalCenter: parent.verticalCenter
        }

        Rectangle {
            id: rect_inputBG

            color: inputBG

            clip: true
            anchors.left: txt_label.right
            anchors.leftMargin: 2
            anchors.right: parent.right
            anchors.rightMargin: 2
            anchors.bottom: parent.bottom
            anchors.bottomMargin: 2
            anchors.top: parent.top
            anchors.topMargin: 2


            TextInput {
                id: txti_input
                text: rect_input.text
                color: inputTextColor
                echoMode: rect_input.echoMode
                font.family: "Monospace"
                font.pixelSize: inputTextFontSize
                horizontalAlignment: TextInput.AlignLeft
                readOnly: rect_input.readOnly
                focus: true

                anchors.verticalCenter: parent.verticalCenter
                anchors.left: parent.left
                anchors.leftMargin: 2
                anchors.right: parent.right
                anchors.rightMargin: 2
            }
//        }

    }

        onFocusChanged: { txti_input.focus = true }
    Binding { target: rect_input; property: "text"; value: txti_input.text}

}
