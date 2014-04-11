import QtQuick 2.0

Flickable {
    id: fa_panel
    width: 200
    height: 100
    interactive: false
    clip: true
    state: "switchPage1"

    contentWidth: rect_panel.width
    contentHeight: rect_panel.height

    property int flickOrientation : Qt.Vertical
    property int flickDuration : 250
    property color color: "#00000000"

    property color titleBG: "#00000000"
    property string title: "Title"
    property int titleTextSize: 12
    property bool titleTextBlod: false
    property font titleFont
    property color titleTextColor: "black"
    property int titleTextRotation: 0
    property bool likeButton : false

    property Item page1 : Rectangle {
        id: rect_title
        parent: it_frontPanel
        width: fa_panel.width
        height: fa_panel.height
        color: titleBG

        Text {
            id: txt_panelTitle
            text: title
            font.pointSize: titleTextSize
            font.bold: titleTextBlod
            wrapMode: Text.WordWrap
            color: titleTextColor
            verticalAlignment: Text.AlignVCenter
            horizontalAlignment: Text.AlignHCenter
            rotation: titleTextRotation
            anchors.fill: rect_title

            MouseArea {
                id: ma_title
                anchors.fill: txt_panelTitle
                cursorShape: Qt.PointingHandCursor
                hoverEnabled: likeButton
                onClicked: {
                    fa_panel.state = "switchPage2"
                }
            }
            states: [
                State {
                    name: "hovered"
                    when: ma_title.containsMouse && ma_title.hoverEnabled
                    PropertyChanges {
                        target: it_frontPanel
                        opacity: 0.8
                    }
                }
            ]
        }
    }


    property Item page2

    property var panelGroup: []

    property string errorText: ""
    property color errorBG: "black"
    property color errorTextColor: "red"
    property int errorTextSize: 10
    property font errorTextFont
    property string errorState: "noError"

    Component.onCompleted: {
        panelGroup.push(fa_panel)
    }

    Rectangle {
        id: rect_panel
//        width: fa_panel.width
//        height: fa_panel.height * 2
        width: { flickOrientation == Qt.Horizontal ? fa_panel.width * 2 : fa_panel.width }
        height: { flickOrientation == Qt.Vertical ? fa_panel.height * 2 : fa_panel.height }
        color: fa_panel.color

        Item {
            id: it_frontPanel
            width: fa_panel.width
            height: fa_panel.height
            x: 0
            y: 0
        }

        Item {
            id: it_ctrlPanel
            width: fa_panel.width
            height: fa_panel.height
            x: { flickOrientation == Qt.Horizontal ? fa_panel.width : 0 }
            y: { flickOrientation == Qt.Vertical ? fa_panel.height : 0 }
        }
        Rectangle {
            id: rect_error
            width: fa_panel.width
            height: fa_panel.height
            x: { flickOrientation == Qt.Horizontal ? fa_panel.width : 0 }
            y: { flickOrientation == Qt.Vertical ? fa_panel.height : 0 }
            color: errorBG
            opacity: 0.85
            state: errorState

            Text {
                anchors.fill: parent
                horizontalAlignment: Text.AlignHCenter
                verticalAlignment: Text.AlignVCenter
                text: errorText
                wrapMode: Text.WordWrap
                font.pointSize: errorTextSize
                color: errorTextColor
            }

            MouseArea {
                anchors.fill: parent
                cursorShape: "PointingHandCursor"
                onClicked: errorState = "noError"
            }

            states: [
                State {
                    name: "noError"
                    PropertyChanges {
                        target: rect_error
                        visible: false
                    }
                },
                State {
                    name: "errorShown"
                    PropertyChanges {
                        target: rect_error
                        visible: true
                    }
                }
            ]
        }
    }

    onPage1Changed: {
        rect_title.destroy();
        page1.parent = it_frontPanel
    }
    onPage2Changed: { page2.parent = it_ctrlPanel }

    states: [
        State {
            name: "switchPage1"
            PropertyChanges {
                target: fa_panel
                contentY: 0
                contentX: 0
            }
        }, State {
            name: "switchPage2"
            PropertyChanges {
                target: fa_panel
//                contentY: fa_panel.height
                contentX: { flickOrientation == Qt.Horizontal ? fa_panel.width : 0 }
                contentY: { flickOrientation == Qt.Vertical ? fa_panel.height : 0 }
            }
            StateChangeScript {
                script: {
                    for (var x in fa_panel.panelGroup) {
                        if (panelGroup[x] !== fa_panel)
                            panelGroup[x].state = "switchPage1"
                    }
                }
            }
        }

    ]
    transitions: Transition {
        NumberAnimation {
            target: fa_panel
            properties: "contentY,contentX"
            duration: flickDuration
        }
    }
}
