function installCmsNav (frame) {
    frame.children ('header').append ('<nav><a href="javascript:void(0)" onclick="addArticle($(\'#' + frame.attr ('id') + '\'))">New article</a><a>Property</a></nav>')
}

var messageQueue = new Array

function refreshAll () {
    closeContentEditor ()
    for (var x in messageQueue) {
        messageQueue [x].cms.refresh (messageQueue [x].signal, messageQueue [x].args)
    }
    $('.notification').hide ()
}

function cmsClient (host, port) {
    var process = $('<div class="waiting">Processing...</div>')
    $('body').append (process)
    process.css ('position', 'fixed')
    process.offset ({top:0, left:0})
    process.hide ()

    var refreshNotification = $('<a class="notification" href="javascript:void(0)" onclick="refreshAll()">29</a>')
    $('body').append (refreshNotification)
    refreshNotification.hide ()

    this.cmsFlow = new flow
    this.cmsFlow.genCall = function () {
        process.width ($(window).width())
        process.height ($(window).height())
        process.show ()
    }
    this.cmsFlow.genReturn = function () {
         process.hide ()
    }

    this.cmsRPC = new RPC (host, port)
    this.cmsFlow.bindRPC (this.cmsRPC)

    framesSetting = new Object
//     categoriesSetting = new Object

    function setFrameTitle (frame, headerText) {
        var header = frame.find ('> header h1')
        if (header.length > 0 && header.html ().length === 0)
            header.html (headerText)
    }
    function attachExtOpt (frame, opt) {
        opt.index = frame.attr ('data-pro-index')
        opt.summary = frame.attr ('data-pro-summary')
        opt.allowPost = frame.attr ('data-pro-allowPost')
        opt.orderBy = frame.attr ('data-pro-order')
        opt._limit = Number (frame.attr ('data-pro-limit'))
        opt.cond = frame.attr ('data-pro-cond')
        opt.link = frame.attr ('data-pro-link')
        return opt;
    }
    function generateArticle (contentPart, article, allowPost, link) {
        var headerHtml = contentPart.header
        if (link)
            headerHtml = '<a href="article.html?article=' + contentPart.id + '">' + headerHtml + '</a>'
        var header = $('<header><h1>' + headerHtml + '</h1></header>')
        if (contentPart.subheader && contentPart.subheader.length > 0)
            header.append ($('<p>' + contentPart.subheader + '</p>'))

        article.append (header)
        article.append ($(contentPart.content))
        if (allowPost) {
            addArticleNav (article)
        }
    }
    function generateArticles (rets, frame) {
        var frameID = rets [1].frameID
        frame.attr ('data-pro-cate', rets [1].cateID)
        var allowPost = rets [1].allowPost
        /*if (frame.attr ('data-pro-allowPost')) {
            rets [1].allowPost = 'true'
        } else if (rets [1].allowPost === 'true') {
            frame.attr ('data-pro-allowPost', true)
        }*/
        for (var x in rets [0]) {
            var artID = rets [0][x].id
            var article = $('<article id="' + frameID + '_art_' + artID + '" data-editable="true"></article>')
            article.attr ('data-id', artID)
            frame.append (article)
            generateArticle (rets [0][x], article, allowPost, rets [1].link)

//             if (rets [1].allowPost == 'true')
//                 active_articleNav_event (article)
        }
    }

    function check_access (sig) {
        var isAccessSig = false
        if (sig === 'ERROR_ACCESS') {
            alert ('ERROR_ACCESS')
            isAccessSig = true
        }

        return isAccessSig
    }

    var p_login = new Proc (this.cmsRPC, function (usr, pwd, cb) {
        done (this.steps [0], function (rets) {
            if (rets [0]) {
                cb ()
            }
        })

        callStep (this.steps [0], usr, pwd, cb)
    }).assignSteps ([
        {t: 'j', o: 'cms', m: 'login' }
    ])
    var p_addContent = new Proc (this.cmsRPC, function (header, subheader, summary, content, category, pcb) {
        done (this.steps [0], function (rets) {
            if (rets [0])
                pcb (rets [1][0].id)
            else
                alert ('error:' + rets[1])
        })

        callStep (this.steps [0], header, subheader, summary, content, category)
    }).assignSteps ([
        {t:'j', o:'cms', m:'addContent'}
    ])
    var p_updateContent = new Proc (this.cmsRPC, function (id, header, subheader, summary, content, category, ucb) {
        done (this.steps [0], function (rets) {
            if (rets [0]) {
                ucb ()
            }
        })

        callStep (this.steps [0], id, header, subheader, summary, content, category)
    }).assignSteps ([
        {t:'j', o:'cms', m:'updateContent'}
    ])
    var p_deleteContent = new Proc (this.cmsRPC, function (article) {
        done (this.steps [0], function (rets) {
            if (rets [0]) {
                p_refreshContents.call (article.parent ()/*, true*/)
            }
        })

        callStep (this.steps [0], article.attr('data-id'))
    }).assignSteps ([
        {t:'j', o:'cms', m:'deleteContent'}
    ])
    var p_initial = new Proc (this.cmsRPC, function (then) {
        done (this.steps [0], function (rets) {
            for (var x in rets [0]) {
                var s = rets[0][x]
                framesSetting [s.frameID] = s
            }
            then ()
        })
        callStep (this.steps [0])
    }).assignSteps ([
        {t:'j', o:'cms', m:'getFrameSetting'}/*,
        {t:'j', o:'cms', m:'getCategoryTerm'}*/
    ])
    var p_getContents = new Proc (this.cmsRPC, function (extCB) {
        done (this.steps [0], function (rets) {
            var stepThis = this
            $('.frame.category,.frame.article').each (function () {
                if (!$(this).attr ('data-pro-unload')) {
                    var s = framesSetting [$(this).attr('id')]
                    if (s) {
                        setFrameTitle ($(this), s.term)
    //                     s = attachExtOpt ($(this), s)
                    } else {
                        s = new Object
                        s.frameID = $(this).attr ('id')
                    }
                    s = attachExtOpt ($(this), s)

                    callStep (stepThis.steps [1], s)
                }
            })
        })
        done (this.steps [1], function (rets) {
            var frame = $('#' + rets [1].frameID)

            generateArticles (rets, frame)
            if (extCB !== undefined)
                extCB (rets)
        })

        callStep (this.steps [0], true)
    }).assignSteps ([
        {t:'j', o:'cms', m:'notification'},
        {t:'j', o:'cms', m:'getContents'}
    ])
    var p_getContentsForManager = new Proc (this.cmsRPC, function () {
        done (this.steps [0], function (rets) {
            var frame = $('#' + rets [1].frameID)
//             var header = frame.children ('header')
//             header.append ('<nav><a href="javascript:void(0)" onclick="addArticle($(\'#' + frame.attr ('id') + '\'))">New article</a><a>Property</a></nav>')
            generateArticles (rets, frame)
        })

        for (var x in framesSetting) {
            var s = framesSetting [x]
            var frame = $('<div class="frame category" id="' + s.frameID + '" data-pro-order="seq" ' +
                            'data-pro-cate="' + s.cateID + '" data-pro-allowPost="true">' +
                                '<header><h1>' + s.term + '</h1></header></div>')
            $('.contents').append (frame)
            installCmsNav (frame)
            s.allowPost = 'true'

            callStep (this.steps [0], s)
        }
    }).assignSteps ([
        {t:'j', o:'cms', m:'getContents'}
    ])
    var p_refreshContents = new Proc (this.cmsRPC, function (frame) {
        done (this.steps [0], function (rets) {
            frame.children ('article').remove ()
            generateArticles (rets, frame)
        })
        var s = framesSetting [frame.attr('id')]
        if (!s)
            s = new Object
        s = attachExtOpt (frame, s)
        callStep (this.steps [0], s)
    }).assignSteps ([
        {t:'j', o:'cms', m:'getContents'}
    ])
    var p_refreshContent = new Proc (this.cmsRPC, function (id) {
        done (this.steps [0], function (rets) {
            var article = $('[data-id=' + rets[0][0].id + ']')
            article.children ().remove ()
            generateArticle(rets [0][0], article, rets [1].allowPost, rets [1].link)
        })

        var frame = $('[data-id=' + id + ']').parent ()
        var s = new Object
        s = attachExtOpt (frame, s)
        s.cond = 'id=' + id
        callStep (this.steps [0], s)
    }).assignSteps ([
        {t:'j', o:'cms', m:'getContents'}
    ])
    var p_getContentForEdit = new Proc (this.cmsRPC, function (id) {
        done (this.steps [0], function (rets) {
            var article = $('[data-id=' + rets[0][0].id + ']')
            var temp = article.html ()
            article.children ().remove ()
            generateArticle (rets [0][0], article, true)
            openContentEditor (article)
            contentBuffer = temp
        })
        var s = {
            cond: 'id=' + id,
            allowPost: true
        }
        callStep (this.steps [0], s)
    }).assignSteps ([
        {t:'j', o:'cms', m:'getContents'}
    ])
    var p_changeSeq = new Proc (this.cmsRPC, function (article) {
        done (this.steps [0], function (rets) {
            p_refreshContents.call (article.parent ()/*, true*/)
        })
        var cateID = article.parent ().attr ('data-pro-cate')
        if (cateID) {
            var seq = prompt ("Enter the article's seq")
            callStep (this.steps [0], article.attr('data-id'), article.parent ().attr ('data-pro-cate'), seq)
        }
    }).assignSteps ([
        {t:'j', o:'cms', m:'changeSeq'}
    ])

    this.makeNav = function (nav) {
        nav.append ($('<a href="index.html">Home</a>'))

        for (var x in framesSetting)
            nav.append ($('<a href="category.html?category=' + framesSetting [x].cateID + '">' + framesSetting [x].term + '</a>'))
    }
    this.refresh = function (sig, args) {
        if (sig === 'refreshCategoryArtiles') {
            p_refreshContents.call ($('[data-pro-cate=' + args [0] + ']'))
        } else if (sig === 'refreshArticle') {
            p_refreshContent.call (args[0])
        } else
            check_access (sig)
    }

    this.loadRelatedArticles = function (container, relArticle, type) {
        var cateID = relArticle.cate_id
        container.attr ('data-pro-cond', 'cate_id=' + cateID + ' AND id!=' + relArticle.id)
        if (type)
            container.attr (type, true)
        container.attr ('data-pro-cate', cateID)
        container.attr ('data-pro-link', true)


        for (var x in framesSetting) {
            if (framesSetting [x].cateID == Number(cateID)) {
                setFrameTitle (container, framesSetting [x].term)
                break
            }
        }
        p_refreshContents.call (container)
    }
    this.loadArticles = function (extCB) {
        var thisCmsClient = this
        onMessage (this.cmsRPC, function (obj, sig, args) {
            if (obj === 'cms') {
                if ($('[data-actived-article=true]').length > 0) {
                    messageQueue.push ({cms: thisCmsClient, signal: sig, args: args})
                    var notification = $('.notification')
                    notification.show ()
                    notification.text (messageQueue.length)
                } else
                    thisCmsClient.refresh (sig, args)
            }
        })
        if (thisCmsClient.cmsRPC.hasConnected ()) {
            p_login.call ('guest', undefined, function () {
                p_initial.call (function () {
                    if (extCB.beforLoadArticles)
                        extCB.beforLoadArticles ()
                    p_getContents.call (extCB.articles)
                    if (extCB.afterLoadArticles)
                        extCB.afterLoadArticles ()
                })
            })
        } else {
            onConnected (thisCmsClient.cmsRPC, function () { thisCmsClient.loadArticles (extCB) })
            connectToHost (thisCmsClient.cmsRPC)
        }
    }

    this.saveArticle = function (article) {
        var contentWrap = $('<div></div>')
        article.children (':not(footer,header,nav)').each (function (){
            var contentPart = $(this).clone ()
            contentWrap.append (contentPart)
        })
        contentWrap.find ('[data-hp-editing=true]').removeAttr ('data-hp-editing')
        contentWrap.find ('[contenteditable=true]').removeAttr ('contenteditable')
        contentWrap.find ('[data-container-editing=true]').removeAttr ('data-container-editing')
        var content = contentWrap.html ()

        var header = article.find ('header h1').html ()
        var subheader = article.find ('header p').html ()
        var summary = ''
        article.find ('span.summary').each (function () {
            summary += $(this).text () + '...'
        })
        if (summary.length > 0)
            summary = '<p>' + summary + '</p>'

        var parentFrame = article.parent ()
        var cateID = parentFrame.attr ('data-pro-cate')
        if (article.attr('data-new')) {
            p_addContent.call (header, subheader, summary, content, cateID, function (id) {
                article.removeAttr ('data-new')
                article.attr ('id', genArticleID (parentFrame.attr ('id'), id))
                article.attr ('data-id', id)
                if (parentFrame.attr ('data-pro-summary')) {
                    article.children ().remove ()
                    generateArticle ({header: header, subheader: subheader, content: summary},article, true)
                }
                contentBuffer=undefined;
                closeContentEditor ()
            })
        } else {
            p_updateContent.call (article.attr('data-id'), header, subheader, summary, content, cateID, function () {
                if (parentFrame.attr ('data-pro-summary')) {
                    article.children ().remove ()
                    generateArticle ({header: header, subheader: subheader, content: summary},article, true)
                }
                contentBuffer=undefined;
                closeContentEditor ()
            })
        }
    }
    this.loginAdmin = function () {
        onMessage (this.cmsRPC, function (obj, sig, args) {
            if (obj === 'cms')
                check_access (sig)
        })

        var thisCmsClient = this;
        if (thisCmsClient.cmsRPC.hasConnected ()) {
            p_login.call ('admin', '781519', function () {
                p_initial.call (function () {
                    p_getContentsForManager.call ()
                })
            })
        } else {
            onConnected (thisCmsClient.cmsRPC, function () { thisCmsClient.loginAdmin ()})
            connectToHost (thisCmsClient.cmsRPC)
        }
    }
//     this.loadArticleForEdit = function (article) {
//         p_getContentForEdit (article)
//     }
    this.deleteContent = function (article) {
        p_deleteContent.call (article)
    }
    this.changeSeq = function (article) {
        p_changeSeq.call (article)
    }
    this.openContentEditor = function (article) {
        var frame = article.parent ()
        if (frame.attr ('data-pro-summary')) {
            console.log ("data-pro-summary")
            p_getContentForEdit.call (article.attr ('data-id'))
        } else
            openContentEditor (article)
    }
}
