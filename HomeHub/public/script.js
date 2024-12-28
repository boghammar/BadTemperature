function getInfo(query, elementId, formatFnc) {
    fetch(query)
        .then(function (response) {
            return response.text();
        })
        .then(function (data) {
            if (undefined === formatFnc) {
                return data;
            }
            return formatFnc(data);
        })
        .then(function (data) {
            document.getElementById(elementId).innerHTML = data;
        })
        .catch(function (error) {
            console.error('Error:', error);
        });
}

// ----------------------------------------------- Devices functions
function formatDevices(devices) {
    devices = JSON.parse(devices);
    console.log(devices);
    console.log(devices.length);
    for (let i = 0; i < devices.length; i++) {
        var btnclass = 'btn btn-off';
        var turnon = true;
        if (devices[i].output) {btnclass = 'btn btn-on'; turnon = false;};
        devices[i] = 
          '<div class="deviceCard">' 
        + '<div class="deviceCardInner">'
        + '  <div class="deviceRows">'
        + '    <div>' + devices[i].name + ' ('+ devices[i].type +') </div>'
        + '    <div id="devstatus_'+devices[i].id+'" class="deviceStatus">' + devices[i].power + ' w ' 
        +        devices[i].rssi + 'db '
        +        devices[i].ip + ' '
        + '    </div>'
        + '  </div>'
        + '  <button class="'+ btnclass +'" id="devbtn_'+devices[i].id+'" onclick="deviceSet('+devices[i].id+','+turnon+')"></button>'
        + '</div>'
        + '</div>';
    }
    devices = devices.join('');
    return devices;
}
// ----------------------------------------------- 
function getDeviceStatus(id) {
    fetch('/api/device/'+id+'/status')
        .then(function (response) {
            return response.text();
        })
        .then(function (data) {
            console.log(data);
            //document.getElementById('devbtn_'+id).innerHTML = 'Data: '+data;
        })
}
// ----------------------------------------------- 
function deviceSet(id, turnon) {
    let temp = turnon ? 'on' : 'off';
    fetch('/api/device/'+id+'/'+temp)
        .then(function (response) {
            return response.text();
        })
        .then(function (data) {
            console.log(data);
            var ret = JSON.parse(data);
            if (ret.success !== undefined) {
                if (!ret.success) {
                    document.getElementById('errormessage').innerHTML = 'Error: ' + ret.message + '<br>';
                } else {
                    let tt = turnon ? 'btn-on' : 'btn-off';
                    document.getElementById('devbtn_'+id).className = 'btn '+tt;
                    document.getElementById('devbtn_'+id).onclick = function () {deviceSet(id,!turnon);};
                }
                /* Obtain power state after 2secs */
                setTimeout(async function() {
                  const resp = await fetch('/api/device/'+id+'/power');
                  const data = await resp.json();
                  document.getElementById('devstatus_'+id).innerHTML = data.power + ' w ' + data.rssi + 'db ' + data.ip;
                }, 5000);
            } else {
                document.getElementById('errormessage').innerHTML = 'Error: ' + JSON.stringify(ret) +  '<br>';
            }
        })
}


// ----------------------------------------------- Execute on load
getInfo('/info?what=hostname', 'hostname');
getInfo('/info?what=version', 'version');
getInfo('/info?what=devices', 'devices', formatDevices);
