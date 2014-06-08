function showDialog(id) {
    $('.dialog').hide()
    $(id).attr("style", "display: flex; display: -webkit-flex");
}
function closeDialog (id, cb)
{
    $(id).hide ()
    if (cb !== undefined)
        cb ()
}

function RPC_waiting () {
    $('#waiting').show()
}
function RPC_finished () {
    $('#waiting').hide()
}

function resizeForm () {
    $(".client_form").height ($(window).height ())
    $('#waiting').width ($(window).width ())
    $('#waiting').height ($(window).height ())
}

$(document).ready(function(){
    $('body').append('<div id="waiting"></div>');
    resizeForm ()
    $(window).resize(resizeForm)
});
