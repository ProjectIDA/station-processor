/* alarm list expansion/contraction */
function display(id) {
    var els = document.getElementsByTagName('div');
    for (var i in els) {
        if (els[i].className == 'alarms') {
            if ((els[i].id == id) && (els[i].style.visibility != 'visible')) {
                els[i].style.visibility = 'visible';
                els[i].style.height = 'auto';
            } else {
                els[i].style.visibility = 'hidden';
                els[i].style.height = '0px';
            }
        }
    }
}
function initialize()
{
    display('NOTHING');
}
