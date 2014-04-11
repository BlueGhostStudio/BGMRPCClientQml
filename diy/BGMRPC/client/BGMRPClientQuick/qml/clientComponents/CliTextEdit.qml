import QtQuick 2.0

Rectangle {
    id: rect_edit
    property string text
    property int textMargin : 0
    property int textMargin_left: -1
    property int textMargin_right: -1
    property int textMargin_top: -1
    property int textMargin_bottom: -1
    property font font
    property var setFocus: function () { txte_edit.forceActiveFocus() }
    property var sellectAll : function () { txte_edit.selectAll() }

    property color editorBG: "white"
    property color editorTextColor: "black"

    Binding { target: rect_edit; property: "text"; when: true; value: txte_edit.text }

    width: 200
    height: 200
    color: editorBG

    Flickable {
        id: flick_edit
        flickableDirection: Flickable.VerticalFlick
        property var textRect: calcRect()

        function calcRect () {
            var rect = new Object
            var dm = textMargin
            var m = [dm,dm,dm,dm]
            if (textMargin_top >= 0)
                m[0] = textMargin_top
            if (textMargin_bottom >= 0)
                m[1] = textMargin_bottom
            if (textMargin_left >= 0)
                m[2] = textMargin_left
            if (textMargin_right >= 0)
                m[3] = textMargin_right

            rect.width = parent.width - m[2] - m[3]
            rect.height = parent.height - m[0] - m[1]
            rect.x = m[2]
            rect.y = m[3]

            return rect
        }

        width: textRect.width
        height: textRect.height
        x: textRect.x
        y: textRect.y

        contentWidth: txte_edit.paintedWidth
        contentHeight: txte_edit.paintedHeight
        clip: true

        TextEdit {
            id: txte_edit
            width: flick_edit.textRect.width
            height: flick_edit.textRect.height
            text: rect_edit.text
            color: editorTextColor
            wrapMode: TextEdit.WrapAtWordBoundaryOrAnywhere
            font: rect_edit.font

            Keys.onReleased: {
                var y = cursorRectangle.y
                var h = flick_edit.height - cursorRectangle.height
                var cy = flick_edit.contentY
                if (y - cy > h)
                    flick_edit.contentY = y - h
                else if (y < cy)
                    flick_edit.contentY = y
            }
        }
    }


}
