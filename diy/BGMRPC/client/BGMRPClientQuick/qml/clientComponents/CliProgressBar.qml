import QtQuick 2.0

Rectangle {
    width: 200
    height: 22
    color: "#999"
    radius: { useRadius ? height / 4 : 0 }

    property int value: 812
    property int maxValue: 1024
    property bool useRadius: true
    property color barColor: "#666"
    property color valueColor: "black"
    property font valueFont
    property double ratio: value / maxValue
    property int percentValue: { Math.round(ratio * 100) }
    property string percentText: percentValue + '%'

    Rectangle {
        id: rect_bar
        x: 3
        y: 3
        width: value / maxValue * (parent.width - 6)
        height: parent.height - 6
        color: barColor
        radius: { useRadius ? height / 4 : 0 }
        Text {
            anchors.verticalCenter: parent.verticalCenter

            x: { (parent.parent.width - 6) * ratio - width * ratio  }
            text: percentText

            color: valueColor
            font: valueFont
        }
    }
}
