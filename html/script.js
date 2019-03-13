var program = [];

$(function() {

})

function param(name) {
    return (location.search.split(name + '=')[1] || '').split('&')[0];
}

function wsConnect() {
    if ('WebSocket' in window) {

        // accept ?target=10.0.0.123 to make a WS connection to another device
        if (target = param('target')) {
            // 
        } else {
            target = document.location.host;
        }
        ws = new WebSocket('ws://' + target + '/ws');
        ws.binaryType = 'arraybuffer';
        ws.onmessage = function(event) {
            //Handle responses here
        };
    }
}

function wsSend(data) {
    ws.send(data);
}

function controlButtonStart(btn) {
    var mode = $("input[name='mode']:checked").val()
    if (mode == 'program') {
        program.push(btn);
        refreshProgram();
    } else {
        var data = {};
        data = "{'mode': '1', 'command': '" + btn + "'}";
        wsSend(data);
    }
}

function controlButtonStop(btn) {
    var data = {};
    data = "{'mode': '0'}";
    wsSend(data);
}

function controlButtonGo() {
    var data = {};
    data = "{'mode': '2', 'program': '" + program.join('') + "'}";
    wsSend(data);
}

function controlButtonBackSpace() {
    program.pop();
    refreshProgram();
}

function controlButtonClear() {
    program = [];
    refreshProgram();
}

function refreshProgram() {
    var progString = '',
        tempstring = '',
        lastChar = '',
        count = 0;
    for (i = 0; i < program.length; i++) {
        if (lastChar == '') {
            // first element;
            count++;
            lastChar = program[i];
            tempstring = lastChar + count;
        } else if (lastChar == program[i]) {
            //repeated
            count++;
            tempstring = lastChar + count;
        } else {
            //char changed
            progString = (progString ? progString + ' > ' + tempstring : progString + tempstring);
            count = 1;
            lastChar = program[i];
            tempstring = lastChar + count;
        }
    }
    progString = (progString ? progString + ' > ' + tempstring : progString + tempstring);
    console.log(progString);
    $('#program').val(progString);
}