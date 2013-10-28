import QtQuick 2.0

Item {
    id: button
    width: txt_btnLabel.width + padding * 2
    height: button.size
    opacity: 1

    property string label: qsTr("Ok")
    property string color: "black"
    property int size : 24
    property int padding: 2
    signal clicked ()

    Rectangle {
        id: rect_btnBorder
        color: "#00ffffff"
        border.color: button.color
        radius: 4
        opacity: 0.6

        anchors.fill: parent

        Text {
            id: txt_btnLabel

            text: button.label
            font.pixelSize: button.height - button.padding * 2
            color: button.color

            anchors.horizontalCenter: parent.horizontalCenter
            anchors.verticalCenter: parent.verticalCenter

            MouseArea {
                id: ma_button
                anchors.fill: parent
                hoverEnabled: true

                onClicked: button.clicked()
            }
        }
    }
    states: [
        State {
            name: "clicked"

            when: ma_button.pressed
            PropertyChanges {
                target: rect_btnBorder
                border.width: 2
            }
            PropertyChanges {
                target: txt_btnLabel
                font.bold: true
            }
        },
        State {
            name: "hovered"

            when: ma_button.containsMouse
            PropertyChanges {
                target: rect_btnBorder
                opacity:1
            }
        }
    ]
}
