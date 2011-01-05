function initialize() {
    var args = new Array();
    args['station'] = null;
    args['channel'] = null;
    var query = window.location.search.substring(1);
    var parms = query.split('&');
    for (var i=0; i < parms.length; i++) {
        var pos = parms[i].indexOf('=');
        if(pos > 0) {
            var key = parms[i].substring(0,pos);
            var val = parms[i].substring(pos+1);
            args[key] = val;
        }
    }

    if ((args['station'] == null) || (args['channel'] == null)) {
        document.getElementById('MAIN').innerHTML = '<H1>Invalid Arguments</H1>';
    } else {
        document.getElementById('MAIN').innerHTML = '<applet code="ofcweb" archive="ofcweb.jar" width="1000" height="350"> <PARAM NAME="ofcdata" VALUE="' + args['station'] + '_ofc.seed.hist"> <PARAM NAME="channel" VALUE="' + args['channel'] + '"> <PARAM NAME="minutespertick" VALUE="1440"> </applet>';
    }
}

