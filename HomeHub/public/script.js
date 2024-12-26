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
    fetch('/device?operation=status&id=' + id)
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
    fetch('/device?operation=turnon&id=' + id+'&turnon='+turnon)
        .then(function (response) {
            return response.text();
        })
        .then(function (data) {
            console.log(data);
            var ret = JSON.parse(data);
            if (ret.success !== undefined) {
                if (!ret.success) {
                    document.getElementById('errormessage').innerHTML = 'Error: ' + ret.message + '<br>';
                }
                /* Obtain power state after 2secs */
                setTimeout(async function() {
                  const resp = await fetch('/device?operation=getPowerState&id=' + id);
                  const data = await resp.json();
                  document.getElementById('devstatus_'+id).innerHTML = data.power + ' w ' + data.rssi + 'db ' + data.ip;
                }, 2000);
            } else {
                document.getElementById('errormessage').innerHTML = 'Error: ' + JSON.stringify(ret) +  '<br>';
            }
        })
}


// ----------------------------------------------- Execute on load
getInfo('/info?what=hostname', 'hostname');
getInfo('/info?what=devices', 'devices', formatDevices);
