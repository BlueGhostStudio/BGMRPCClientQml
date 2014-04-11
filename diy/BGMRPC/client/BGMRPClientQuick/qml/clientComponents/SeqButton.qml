import QtQuick 2.0

Rectangle {
    id: rect_button
    width: 100
    height: 62

    property string label: "ok"
    property color labelTextColor: "black"
    property int labelSize: 12
    property bool labelBold: false
    signal clicked ()

    Text {
        text: label
        color: labelTextColor
        font.pointSize: labelSize
        font.bold: labelBold

        anchors.fill: parent
        horizontalAlignment: Text.AlignHCenter
        verticalAlignment: Text.AlignVCenter
    }
    MouseArea {
        id: ma_button
        anchors.fill: parent
        hoverEnabled: true

        cursorShape: "PointingHandCursor"
        onClicked: rect_button.clicked ()
    }

    states: [
        State {
            name: "clicked"
            when: ma_button.pressed
            PropertyChanges {
                target: rect_button
                opacity: 0.8
            }
        },
        State {
            name: "hovered"
            when: ma_button.containsMouse
            PropertyChanges {
                target: rect_button
                opacity: 0.8
            }
        }

    ]
}
