var container_filter = 'article,section,header,ol,ul'

function parentContainer (sel)
{
    return sel.parents (container_filter).first ()
}



function setEditing (sel, forceResetContainerNavBar) {
//     if (sel.index () === -1) {
//         resetEditNavBar (sel, 2)
//     } else{
    $('[data-hp-editing=true]').removeAttr ('data-hp-editing')
    if (sel.index () !== -1) {
//         $('#editing').removeAttr('id')
//         sel.attr('id', 'editing')
        sel.attr ('data-hp-editing', true)
//         resetEditNavBar (sel, 3)
    }

    resetEditNavBar (sel, 3)
    var container = $('article [data-container-editing=true]')
    if (forceResetContainerNavBar !== undefined
        && forceResetContainerNavBar
        && container.index () != -1)
        resetEditNavBar (container, 1)
}

function activeEdit (sel, forceResetContainerNavBar) {
    sel.focus ()
    setEditing (sel, forceResetContainerNavBar)
}

function gotoNext (sel, filter) {
    return sel.nextAll(filter).first()
}
function gotoPrev (sel, filter) {
    return sel.prevAll(filter).first()
}

function swapHP (sel, isUp) {
    var target = isUp ? sel.prev (':not(header)') : sel.next (':not(footer)')
    if (target.index () !== -1) {
        var clone = sel.clone (true)
        clone.html (sel.text ())
        sel.remove ()

        isUp ? clone.insertBefore (target) : clone.insertAfter (target)

        // clone.focus()
        activeEdit (clone)
    }
}

function htmlToEditText (sel) {
    sel.text (sel.html())
}

function resetEditNavBar (sel, op)
{
    var topNav = $('.content_edit_nav.top')
    var bottomNav = $('.content_edit_nav.bottom')
    var articleNav = $('.content_edit_nav.article')
    var sectionTopNav = $('.container_edit_nav.top')
    var sectionBottomNav = $('.container_edit_nav.bottom')

    if (sel.index () === -1) {
        topNav.hide ()
        bottomNav.hide ()
    } else {
        if (sel.attr('data-hp-editing') !== undefined) {
            var which = sel.parents (container_filter).first ().get(0).nodeName
            topNav.attr ('data-which', which)
            bottomNav.attr ('data-which', which)
        } else if (sel.attr('data-container-editing') !== undefined) {
            var which = sel.first ().get (0).nodeName
            sectionTopNav.attr ('data-which', which)
            sectionBottomNav.attr ('data-which', which)
        }
        var selTop = sel.offset ().top
        var selLeft = sel.offset ().left
        var selWidth = sel.outerWidth ()
        var selHeight = sel.outerHeight ()
        switch (op) {
        case -1: // initial
            $('.content_edit_nav:not(.article)').hide ()
            articleNav.css ('display', 'inline-block')
            var header = sel.find ('>header')
            articleNav.offset (
                {
                    top: header.offset ().top + header.innerHeight () + 5,
                    left: header.offset ().left
                })
            break
        case 0: // click article
            sectionTopNav.hide ()
            sectionBottomNav.hide ()
            break;
        case 1: // click section
            sectionTopNav.css ('display', 'inline-block')
            sectionTopNav.offset (
                {
                    top: selTop - sectionTopNav.outerHeight (),
                    left: selLeft + selWidth - sectionTopNav.outerWidth ()
                })
            sectionBottomNav.css ('display', 'inline-block')
            sectionBottomNav.offset (
                {
                    top: selTop + selHeight + 2,
                    left: selLeft
                })
            break;
        case 2: // no editing
            topNav.hide ()
            bottomNav.hide ()
            break;
        case 3: // focusin hp
            topNav.css('display', 'inline-block')
            topNav.offset (
                {
                    top: selTop - topNav.outerHeight() - 2,
                    left: selLeft
                })

            bottomNav.css('display', 'inline-block')
            bottomNav.offset (
                {
                    top: selTop + selHeight + 2,
                    left: selLeft + selWidth - bottomNav.outerWidth()
                })

            break
        case 4: // resize
            bottomNav.css('display', 'inline-block')
            bottomNav.offset (
                {
                    top: selTop + selHeight + 2,
                    left: selLeft + selWidth - bottomNav.outerWidth ()
                })
            var container = $('article [data-container-editing=true]')
            if (container.index () != -1)
                resetEditNavBar (container, 1)

            break
        }
    }
}
function editTextToHtml (sel) {
    var p_html = sel.text()
    if (p_html.length > 0) {
        var newP = $('<p contenteditable="true"></p>')
        newP.html(p_html)
        return newP
    } else
        return undefined
}
function unionParagraph (sel, target, isBefore) {
    if (target.index() >= 0) {
        if (isBefore)
            target.html(target.html() + ' ' + sel.text())
        else
            target.html(sel.text() + ' ' + target.html())

        sel.remove()
        target.focus()
    }
}

function changeHeaderLevel (sel, level)
{
    var newH = $('<h' + level + ' contenteditable="true"></h' + level + '>')
    newH.html (sel.text ())
    newH.insertAfter (sel)
    sel.remove ()
    reset_h_event (newH)
    activeEdit (newH)
//     newH.focus ()
}
function increaseHeaderLevel (sel)
{
    level = Number(sel.get(0).nodeName[1]) - 1
    if (level > 0)
        changeHeaderLevel (sel, level)
}
function reductionHeaderLevel (sel) {
    level = Number(sel.get(0).nodeName[1]) + 1
    if (level <= 6)
        changeHeaderLevel (sel, level)
}

/*
 * 增加相关
 */
function newParagraph () {
    return $('<p contenteditable="true"></p>')
}
function newHeader () {
    return $('<h1 contenteditable="true"></h1>')
}
function newSection () {
    return $('<section></section>')
}
function newCode () {
    return $('<pre contenteditable="true"></pre>')
}
function insertAfterCurrentContainer (insertCB) {
    var sel = $('[data-container-editing=true]')

    sel.removeAttr ('data-container-editing')
    resetEditNavBar (sel, 0)
    var targetContainer = parentContainer (sel)
    targetContainer.attr ('data-container-editing', true)
    insertCB (sel)
}
function addParagraph (direction) {
    switch (Number(direction)) {
    case 0:
    case 1:
        insertHP ($('[data-hp-editing=true]'),
                  newParagraph (), reset_p_event,
                  direction == 0 ? true : false)
        break
    case 2:
        insertAfterCurrentContainer (function (sel) {
                                         insertHP (sel, newParagraph (),
                                                   reset_p_event, false)
                                     })
        break
    }
}
function addHeader (direction) {
    switch (Number (direction)) {
    case 0:
    case 1:
        insertHP ($('[data-hp-editing=true]'),
                  newHeader (), reset_h_event,
                  direction == 0 ? true : false)
        break
    case 2:
        insertAfterCurrentContainer (function (sel) {
                                         insertHP (sel, newHeader (),
                                                   reset_h_event, false)
                                     })
        break
    }
}
function addSection (direction) {
    switch (Number (direction)) {
    case 0:
    case 1:
        insertSection ($('[data-hp-editing=true]'),
                           newSection (), reset_section_event,
                           direction == 0 ? true : false)
        break
    case 2:
        insertAfterCurrentContainer (function (sel) {
                                         insertSection (sel, newSection (),
                                                        reset_section_event, false)
                                     })
        break
    }
}
function addCode (direction) {
    switch (Number (direction)) {
    case 0:
    case 1:
        insertHP ($('[data-hp-editing=true]'),
                  newCode (), reset_code_event,
                  direction == 0 ? true : false)
        break
    case 2:
        insertAfterCurrentContainer (function (sel) {
            insertHP (sel, newCode (),
                      reset_code_event, false)
        })
        break
    }
}
function addSectionHeader () {
    insertSectionHeader ()
}
function insertDefaultElem (sel, e, reset_event, isBefore) {
    if (sel.index () !== -1) {
        if (isBefore)
            e.insertBefore (sel)
        else
            e.insertAfter (sel)
    } else {
        var container = $('[data-container-editing=true]')
        container.append (e)
    }
    reset_event(e)
}
function insertHP (sel, e, reset_event, isBefore) {
    insertDefaultElem (sel, e, reset_event, isBefore)
    activeEdit (e, true)
}
function insertSection (sel, e, reset_event, isBefore) {
    insertDefaultElem (sel, e, reset_event, isBefore)
    e.click ()
}
function insertSectionHeader () {
    var container = $('[data-container-editing=true]')
    if (container.get (0).nodeName === 'SECTION') {
        var header = container.find ('header')
        if (header.index() === -1) {
            header = $('<header contenteditable="true"></header>')
            container.prepend(header)
            reset_section_header_event (header)
        }

        header.focus ()
        activeEdit ($(), true)
    }
}
// function insertParagraph (sel, isBefore, e) {
//     if (e === undefined)
//         e = $('<p contenteditable="true"></p>')
//
//     insertHP (sel, e, reset_p_event, isBefore)
//     var newP = $('<p contenteditable="true"></p>')
//     if (isBefore)
//         newP.insertBefore(sel)
//     else
//         newP.insertAfter(sel)
//     reset_p_event (newP)
//     activeEdit (newP, true)
// }
// function insertHeader (sel, isBefore, e) {
//     if (e === undefined)
//         e = $('<h1 contenteditable="true"></h1>')
//     insertHP (sel, e, reset_h_event, isBefore)
//     var newH = $('<h1 contenteditable="true"></h1>')
//     if (isBefore)
//         newH.insertBefore (sel)
//     else
//         newH.insertAfter (sel)
//     reset_h_event (newH)
//     activeEdit (newH, true)
// }

/*
 * 事件相关
 */
function initial_event (Ps, Hs, Cos) {
    $(document).on('keyup', global_keyup)
    reset_p_event (Ps)
    reset_h_event (Hs)
    reset_code_event (Cos)

    $('[data-actived-article=true]').on ('click', function (event) {
        event.stopPropagation ()
        $('[data-container-editing=true]').removeAttr ('data-container-editing')
        $(this).attr ('data-container-editing', true)
        resetEditNavBar ($(this), 0)
    })

    reset_section_event ($('[data-actived-article=true] section'))

    var Hh = $('[data-actived-article=true] header h1')
    Hh.on('focusin', HhHp_focusin)
    Hh.on('focusout', Hh_focusout)
    reset_Hp_event (Hh.next ('p'))
}
function clear_event (Es) {
    $(document).off('keyup')
//     $(document).off('keydown')
    Es.off('heightChanged')
    clear_edit_event (Es)
    $('article,section').off('click')

//     var Hh = $('[data-actived-article=true] header h1')
//     Hh.off ('focusin')
//     Hh.off ('focusout')
//     var Hp = Hh.next ('p')
//     Hp.off ('focusin')
//     Hp.off ('focusout')
}
function global_keyup (event) {
    if (!event.altKey && !event.ctrlKey && !event.shiftKey) {
        switch (event.which) {
        case 73:
            if ($(':focus').index () === -1)
                $('[data-hp-editing=true]').focus()
            break
        case 27:
            $(':focus').blur()
            break
        }
    }
}

function reset_section_header_event (header)
{
    header.on ('focusin', HhHp_focusin)
    header.on ('focusout', section_header_focusout)
    header.on ('keyup', function () { activeEdit ($(), true) })
}
function section_header_focusout () {
    var Hh = $(this)
    var html = $(this).text ()
    if (html.length > 0)
        Hh.html (html)
    else
        Hh.remove ()

    activeEdit ($(), true)
}
function HhHp_focusin () {
    htmlToEditText ($(this))
    activeEdit ($())
}
function Hh_focusout () {
    var Hh = $(this)

    var line = 0
    var HpHtml = ''
    Hh.contents ().each (function () {
        var html = $(this).text ()
        if (line === 0)
            Hh.html (html)
        else
            HpHtml += html

        line++
    })

    if (HpHtml.length > 0) {
        var Hp = Hh.next ('p')
        if (Hp.index () !== -1)
            Hp.html (HpHtml + Hp.html ())
        else {
            Hp = $('<p contenteditable="true"></p>')
            Hp.html (HpHtml)
            Hp.insertAfter (Hh)
            reset_Hp_event (Hp)
        }
    }
}
function reset_Hp_event (Hp) {
    Hp.on ('focusin', HhHp_focusin)
    Hp.on ('focusout', Hp_focusout)
}
function Hp_focusout () {
    var HpHtml = $(this).text ()
    var Hh = $('[data-actived-article=true] header h1')

    if (HpHtml.length > 0)
        $(this).html (HpHtml)
    else
        $(this).remove ()
}


function hp_focusin () {
    $(this).attr('data-height', $(this).height ())
    htmlToEditText ($(this))
    setEditing ($(this))
//     var parentContainer = $(this).parents ('article,section,header').first ()
//     if (parentContainer.attr ('data-container-editing') != true) {
//         $('[data-container-editing=true]').removeAttr ('data-container-editing')
//         parentContainer.attr ('data-container-editing', true)
//     }
}
function p_focusout () {
    var curP = $(this)
    var html = $('<div>' + $(this).html () + '</div>')
    html.contents().each(function () {
        var newP = editTextToHtml ($(this))
        if (newP !== undefined) {
            newP.insertAfter(curP)
            reset_p_event(newP)
            curP = newP
        }
    })
    $(this).remove()
    setEditing (curP, true)

//     $('.editingArticle .content_edit_nav.top').hide ()
//     $('.editingArticle .content_edit_nav.bottom').hide ()
}
function p_keyup (event) {
    var height = $(this).height ()
    if (height != $(this).attr('data-height')) {
        $(this).attr('data-height', height)
        $(this).trigger ('heightChanged', [$(this)])
    }
    if (event.altKey && !event.ctrlKey && !event.shiftKey) {
        switch (event.which) {
        case 33: // alt+page up 与上段合并
            unionParagraph ($(this), gotoPrev ($(this),'p'),true)
            break;
        case 34: // alt+page down 与下段合并
            unionParagraph ($(this), gotoNext ($(this),'p'),false)
            break;
        case 38: // alt+up 编辑上一段
            var prevP = gotoPrev ($(this), 'p')
            if (prevP.index () !== -1) activeEdit (prevP)
            break
        case 40: // alt+down 编辑下一段
            var nextP = gotoNext ($(this), 'p')
            if (nextP.index () !== -1) activeEdit (nextP)
            break
        case 46: // alt+del 删除当前段
            deleteParagraph ($(this))
            break
        case 188: // alt+, 在当前段前插入一段
//             insertParagraph ($(this), true)
            insertHP ($(this), newParagraph (), reset_p_event, true)
            break
        case 190: // alt+. 在当前段后插入一段
//             insertParagraph ($(this), false)
            insertHP ($(this), newParagraph (), reset_p_event, false)
            break
        }
    } else if (event.ctrlKey && !event.altKey && !event.shiftKey) {
        switch (event.which) {
        case 38: // ctrl+up 与前一段交换
            swapHP ($(this), true)
            break
        case 40: // ctrl+down 与后一段交换
            swapHP ($(this), false)
            break
        case 188: // ctrl+, 在当前段前插入一标题
//             insertHeader ($(this), true)
            insertHP ($(this), newHeader (), reset_h_event, true)
            break
        case 190: // ctrl+. 在当前段标题后插入一标题
            insertHP ($(this), newHeader (), reset_h_event, false)
            break
        }
    } else if (!event.altKey && event.ctrlKey && event.shiftKey) {
        switch (event.which) {
        case 38:
            var prevHeader = gotoPrev ($(this), ':header')
            if (prevHeader.index () !== -1) activeEdit (prevHeader)
            break
        case 40:
            var nextHeader = gotoNext ($(this), ':header')
            if (nextHeader.index () !== -1) activeEdit (nextHeader)
            break
        }
    }
}
function h_keyup (event) {
    var height = $(this).height ()
    if (height != $(this).attr('data-height')) {
        $(this).attr('data-height', height)
        $(this).trigger ('heightChanged', [$(this)])
    }
    if (event.altKey && !event.ctrlKey && !event.shiftKey) {
        switch (event.which) {
        case 33:
            increaseHeaderLevel ($(this))
            break
        case 34:
            reductionHeaderLevel ($(this))
            break
        case 38:
            var prevHeader = gotoPrev ($(this), ':header')
            if (prevHeader.index () !== -1) activeEdit (prevHeader)
            break
        case 40:
            var nextHeader = gotoNext ($(this), ':header')
            if (nextHeader.index () !== -1) activeEdit (nextHeader)
            break
        case 46: // alt+del 删除当前标题
            deleteHeader ($(this), ':header')
            break
        case 188: // alt+, 在当前标题前插入一段
            insertHP ($(this), newParagraph (), reset_p_event, true)
            break
        case 190: // alt+. 在当前段后插入一段
            insertHP ($(this), newParagraph (), reset_p_event, false)
            break
        }
    } else if (!event.altKey && event.ctrlKey && !event.shiftKey) {
        switch (event.which) {
        case 38:
            swapHP ($(this), true)
            break
        case 40:
            swapHP ($(this), false)
            break
        case 188: // ctrl+, 在当前标题前插入一标题
//             insertHeader ($(this), true)
            insertHP ($(this), newHeader (), reset_h_event, true)
            break
        case 190: // ctrl+. 在当前标题后插入一标题
//             insertHeader ($(this), false)
            insertHP ($(this), newHeader (), reset_h_event, false)
            break
        }
    } else if (!event.altKey && event.ctrlKey && event.shiftKey) {
        switch (event.which) {
        case 38: // ctrl+shift+up 编辑上一段
            var prevP = gotoPrev ($(this), 'p')
            if (prevP.index () !== -1) activeEdit (prevP)
            break
        case 40: // ctrl+shift+down 编辑下一段
            var nextP = gotoNext ($(this), 'p')
            if (nextP.index () !== -1) activeEdit (nextP)
            break
        case 49: // h1
        case 192:
            changeHeaderLevel ($(this), 1)
            break
        case 50: // h2
            changeHeaderLevel ($(this), 2)
            break
        case 51: // h3
            changeHeaderLevel ($(this), 3)
            break
        case 52: // h4
            changeHeaderLevel ($(this), 4)
            break
        case 53: // h5
            changeHeaderLevel ($(this), 5)
            break
        case 54: // h6
            changeHeaderLevel ($(this), 6)
            break
        }
    }
}
function h_focusout () {
//     console.log ('[' + $(this).text () + ']')
//     var headerHtml = ''
//     $(this).contents ().each (function () {
//         headerHtml += ' '
//         headerHtml += $(this).text ()
//     })
    var headerHtml = $(this).text ()
    headerHtml = headerHtml.trim ()
    if (headerHtml.length === 0) {
        $(this).remove ()
//         resetEditNavBar ($(this), -1)
        setEditing ($(this), true)
    } else
        $(this).html (headerHtml)

//     $('.editingArticle .content_edit_nav.top').hide ()
//     $('.editingArticle .content_edit_nav.bottom').hide ()
}
function c_keyup (event) {
    var height = $(this).height ()
    if (height != $(this).attr('data-height')) {
        $(this).attr('data-height', height)
        $(this).trigger ('heightChanged', [$(this)])
    }
}
function c_focusout (event) {
    if ($(this).text ().length === 0) {
        $(this).remove ()
        setEditing ($(), true)
    } else {
        var codeHtml = ''
        $(this).contents().each(function () {
            var text = ($(this).text ())
            if (text.length === 0)
                codeHtml += '\n'
            else
                codeHtml += $(this).text ()
        })
        console.log (codeHtml.length)
        $(this).html (codeHtml)
    }
}
function clear_edit_event (Es) {
    Es.off("keyup")
    Es.off("focusin")
    Es.off("focusout")
    Es.off('paste')
}
function pasteText (e) {
    $(this).text (e.originalEvent.clipboardData.getData('text/plain'))
    return false
}
function reset_p_event (Ps) {
    clear_edit_event (Ps)
    Ps.on('heightChanged', function (event, sel) { resetEditNavBar (sel, 4) })
    Ps.on('keyup', p_keyup)
    Ps.on('focusin', hp_focusin)
    Ps.on('focusout', p_focusout)
    Ps.on('paste', pasteText)
}
function reset_h_event (Hs) {
    clear_edit_event (Hs)
    Hs.on('heightChanged', function (event, sel) { resetEditNavBar (sel, 4) })
    Hs.on('keyup', h_keyup)
    Hs.on('focusin', hp_focusin)
    Hs.on('focusout', h_focusout)
    Hs.on('paste', pasteText)
}
function reset_code_event (Cos) {
    console.log ('reset_code_event')
    clear_edit_event (Cos)
    Cos.on ('heightChanged', function (event, sel) { resetEditNavBar (sel, 4) })
    Cos.on ('keyup', c_keyup)
    Cos.on ('focusin', hp_focusin)
    Cos.on ('focusout', c_focusout)
    Cos.on ('paste', pasteText)
}
function reset_section_event (section) {
    section.off ('click')
    section.on ('click', function (event) {
        event.stopPropagation ()

        $('[data-container-editing=true]').removeAttr ('data-container-editing')
        $(this).attr ('data-container-editing', true)

        resetEditNavBar ($(this), 1)

        var sel = $(this).find ('[data-hp-editing]')
        if (sel.index () === -1)
            activeEdit ($())
    })
    reset_section_header_event (section.find ('header'))
}

function active_articleNav_event (article) {
//     var article_nav = $('.article_nav')

//     article.hover (function () {
//         var editable = $(this).attr ('data-editable')
//         var isNew = $(this).attr ('data-new')
//         if (editable || isNew) {
//             if ($(this).attr ('data-actived-article')) {
//                 $('#a_close,#a_save,#a_property').show ()
//                 $('#a_edit').hide ()
//                 $('#a_changeSeq').hide ()
//                 $('#a_delArticle').hide ()
//             } else {
//                 $('#a_edit').show ()
//                 $('#a_changeSeq').show ()
//                 $('#a_delArticle').show ()
//                 $('#a_close,#a_save,#a_property').hide ()
//             }
//             article_nav.show ()
//             article_nav.attr ('data-artID', $(this).attr ('id'))
//             article_nav.offset (
//                 {
//                     top: $(this).offset ().top + article_nav.height () + 5,
//                     left: $(this).offset ().left + $(this).width () - article_nav.width ()
//                 })
//         }
//     }, function (event) {
// //         article_nav.hide ()
//         if (event.relatedTarget.parentNode.className !== "article_nav")
//             article_nav.hide ()
//     })
//     article_nav.hover (undefined, function () { article_nav.hide () })
}

/*
 * 删除相关
 */
function deleteHP (sel, filter) {
    var nextHP = gotoNext (sel, filter)
    var preHP = gotoPrev (sel, filter)
    sel.remove ()
    if (nextHP.index () !== -1)
        activeEdit (nextHP)
//         nextHP.focus ()
    else
        activeEdit (preHP)
//         preHP.focus ()
}
function deleteParagraph (sel) {
    deleteHP (sel, 'p')
//     var nextP = gotoNext (sel, 'p')
//     var preP = gotoPrev (sel, 'p')
//     sel.remove()
//     if (nextP.index() >= 0)
//         nextP.focus()
//     else if (preP.index() >= 0)
//         preP.focus()
}
function deleteHeader (sel) {
    deleteHP (sel, 'p,:header')
}
function deleteSubSection () {
    var container = $('[data-container-editing]')
    var nextContainer = gotoNext (container, container_filter)
    var preContainer = gotoPrev (container, container_filter)
    container.remove ()
    if (nextContainer.index () !== -1) {
        nextContainer.click ()
        activeEdit (nextContainer.children ('p').first ())
    } else if (preContainer.index () !== -1) {
        preContainer.click ()
        activeEdit (preContainer.children ('p').first ())
    } else {
        $('data-actived-article').click ()
        activeEdit ($())
    }
}

var contentBuffer = undefined

function openContentEditor (article) {
    closeContentEditor ()
    contentBuffer = article.html ()

    article.find ('.a_edit, .a_changeSeq, .a_delArticle').hide ()
    article.find ('.a_close,.a_save,.a_property').show ()

    article.attr ('data-actived-article', true)
    article.attr ('data-container-editing', true)

    // 内容段落初始
    var Ps = article.find ('p')
    Ps.attr('contenteditable', true)

    // 标题初始
    var Hs = article.find (':header')
    Hs.attr('contenteditable', true)
    var sHeaders = article.find ('section header')
    sHeaders.attr('contenteditable', true)

    // code 初始
    var Cos = article.find ('pre')
    Cos.attr ('contenteditable', true)

    Ps = article.find ('>p,:not(header) p')
    Hs = article.find ('>:header,:not(header) :header')

    initial_event (Ps,Hs,Cos)
    resetEditNavBar (article, -1)

    activeEdit (Ps.first ())
}

function closeContentEditor () {
    console.log ('closeContentEditor')
    $(':focus').blur()
    $('.content_edit_nav').hide ()
    $('.container_edit_nav').hide ()

    var article = $('[data-actived-article=true]')
    article.find ('.a_edit, .a_changeSeq, .a_delArticle').show ()
    article.find ('.a_close,.a_save,.a_property').hide ()
    if (article.length > 0) {
        if (article.attr ('data-new'))
            article.remove ()
        else if (contentBuffer)
            article.html (contentBuffer)
    }
    article.removeAttr ('data-actived-article')

    $('[data-container-editing=true]').removeAttr ('data-container-editing')


    // 重置内容段落和标题
    var Es = article.find ('[contenteditable=true]')
    $('[contenteditable=true]').removeAttr('contenteditable')
    $('[data-hp-editing=true]').removeAttr ('data-hp-editing')


    clear_event (Es)
}

function cmsRPC (sel) {
    var cms = sel.attr ('data-pro-cms')
    if (!cms)
        cms = sel.parents ('[data-pro-cms]').attr ('data-pro-cms')

    return eval (cms)
}
function cmsRPC_openContentEditor (sel) {
    cmsRPC (sel).openContentEditor (sel)
}
function cmsRPC_saveArticle (sel) {
    cmsRPC (sel).saveArticle (sel)
}
function cmsRPC_deleteArticle (sel) {
    cmsRPC (sel).deleteContent (sel)
}
function cmsRPC_changeSeq (sel) {
    cmsRPC (sel).changeSeq (sel)
}
function initialContentEditor () {
    var content_edit_nav = $(
        '<nav class="content_edit_nav">' +
            '<a id="a_header" href="javascript:void(0)" onclick="addSectionHeader()">Header</a>' +
            '<a id="a_p" href="javascript:void(0)" onclick="addParagraph($(this).attr(\'data-direction\'))">P</a>' +
            '<a id="a_h" href="javascript:void(0)" onclick="addHeader($(this).attr(\'data-direction\'))">H</a>' +
            '<a id="a_code" href="javascript:void(0)" onclick="addCode($(this).attr(\'data-direction\'))">code</a>' +
            '<a id="a_section" href="javascript:void(0)" onclick="addSection($(this).attr(\'data-direction\'))">SECTION</a>' +
            '<a id="a_ol">OL</a>' +
            '<a id="a_ul">UL</a>' +
            '<a id="a_img">IMG</a>' +
            '<a id="a_delSection" href="javascript:void(0)" onclick="deleteSubSection ()">[X]</a>' +
        '</nav>'
    )
    var article_edit_nav = content_edit_nav.clone ()
    article_edit_nav.addClass ("article")
    article_edit_nav.children ('a').attr ('data-direction', 1)

//     var article_nav = $(
//                 '<nav class="article_nav">' +
//                     '<a id="a_edit" href="javascript:void(0)" ' +
//                         'onclick="cmsRPC_openContentEditor($(\'#\' + $(this).parent ().attr (\'data-artID\')))">' +
//                         'Edit</a>' +
//                     '<a id="a_delArticle" href="javascript:void(0)" ' +
//                         'onclick="cmsRPC_deleteArticle ($(\'#\' + $(this).parent ().attr (\'data-artID\')))">' +
//                         'Delete</a>' +
//                     '<a id="a_changeSeq" href="javascript:void(0)" ' +
//                         'onclick="cmsRPC_changeSeq($(\'#\' + $(this).parent ().attr (\'data-artID\')))">' +
//                         'Sequence</a>' +
//                     '<a id="a_close" href="javascript:void(0)" ' +
//                         'onclick="closeContentEditor ()">' +
//                         'Close</a>' +
//                     '<a id="a_save" href="javascript:void(0)" ' +
//                         'onclick="cmsRPC_saveArticle($(\'#\' + $(this).parent ().attr (\'data-artID\')))">' +
//                         'Save</a>' +
//                     '<a id="a_property">Property</a>' +
//                 '</nav>'
//             )
//     article_nav.hide ()


    content_edit_nav_top = content_edit_nav.clone ()
    content_edit_nav_top.addClass ("top")
    content_edit_nav_top.children ('a').attr ('data-direction', 0)

    content_edit_nav_bottom = content_edit_nav.clone ()
    content_edit_nav_bottom.addClass ("bottom")
    content_edit_nav_bottom.children ('a').attr ('data-direction', 1)

    var section_edit_nav_top = content_edit_nav.clone ()
    section_edit_nav_top.removeClass ('content_edit_nav')
    section_edit_nav_top.addClass ("container_edit_nav top")
    section_edit_nav_top.children ('a').attr ('data-direction', 1)
    var section_edit_nav_bottom = content_edit_nav.clone ()
    section_edit_nav_bottom.removeClass ('content_edit_nav')
    section_edit_nav_bottom.addClass ('container_edit_nav bottom')
    section_edit_nav_bottom.children ('a').attr ('data-direction', 2)


    $('body').prepend (article_edit_nav)
    //$('body').prepend (article_nav)
    $('body').prepend (content_edit_nav_top)
    $('body').prepend (content_edit_nav_bottom)
    $('body').prepend (section_edit_nav_top)
    $('body').prepend (section_edit_nav_bottom)

    /*$('.layout.main .contents .category > header').each (function () {
        var frameID = $(this).parent ().attr('id')
        var newable = $(this).parent ().attr('data-pro-newable')
        if (frameID !== undefined && newable)
            $(this).append ('<nav><a href="javascript:void(0)" onclick="addArticle($(\'#' + frameID + '\'))">New article</a><a>Property</a></nav>')
    })*/
}

function addArticleNav (article) {
    var article_nav = $('<nav class="article_nav">' +
            '<a class="a_edit" href="javascript:void(0)" ' +
                'onclick="cmsRPC_openContentEditor($(this).parents (\'article\'))">' +
                'Edit</a>' +
            '<a class="a_delArticle" href="javascript:void(0)" ' +
                'onclick="cmsRPC_deleteArticle ($(this).parents (\'article\'))">' +
                'Delete</a>' +
            '<a class="a_changeSeq" href="javascript:void(0)" ' +
                'onclick="cmsRPC_changeSeq($(this).parents (\'article\'))">' +
                'Sequence</a>' +
            '<a class="a_close" href="javascript:void(0)" ' +
                'onclick="closeContentEditor ()">' +
                'Close</a>' +
            '<a class="a_save" href="javascript:void(0)" ' +
                'onclick="cmsRPC_saveArticle($(this).parents (\'article\'))">' +
                'Save</a>' +
            '<a class="a_property">Property</a>' +
        '</nav>')

    article.prepend (article_nav)
    article.find('.a_edit, .a_changeSeq, .a_delArticle').show ()
    article.find('.a_close, .a_save, .a_property').hide ()
}
function addArticle (frame) {
    var article = $('<article id="art_new" data-editable="true" data-new="true"><header><h1></h1></header></article>')
//     article.insertAfter (frame.children ('article').last ())
    frame.append (article)
    addArticleNav (article)
//     setArticleNav (article, 'art_new')
    active_articleNav_event (article)
    openContentEditor (article)
}
function genArticleID (frameID, articleID) {
    return frameID + '_art_' + articleID
}
