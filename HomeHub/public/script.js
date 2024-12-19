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
    console.log(devices.Devices);
    console.log(devices.Devices.length);
    for (let i = 0; i < devices.Devices.length; i++) {
        var btnclass = 'btn btn-off';
        var turnon = true;
        if (devices.Devices[i].output) {btnclass = 'btn btn-on'; turnon = false;};
        devices.Devices[i] = 
          '<div class="deviceCard">' 
        + '<div class="deviceCardInner">'
        + '  <div class="deviceRows">'
        + '    <div>' + devices.Devices[i].name + ' ('+ devices.Devices[i].type +') </div>'
        + '    <div id="devstatus_'+devices.Devices[i].id+'" class="deviceStatus">' + devices.Devices[i].power + ' w ' 
        +        devices.Devices[i].rssi + 'db '
        +        devices.Devices[i].ip + ' '
        + '    </div>'
        + '  </div>'
        + '  <button class="'+ btnclass +'" id="devbtn_'+devices.Devices[i].id+'" onclick="deviceSet('+devices.Devices[i].id+','+turnon+')"></button>'
        + '</div>'
        + '</div>';
    }
    devices = devices.Devices.join('');
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
            if (ret.was_on !== undefined) {
                if (ret.was_on) {
                    document.getElementById('devbtn_'+id).className = 'btn btn-off';
                    document.getElementById('devbtn_'+id).onclick = function () {deviceSet(id,true);};
                }
                else {
                    document.getElementById('devbtn_'+id).className = 'btn btn-on';
                    document.getElementById('devbtn_'+id).onclick = function () {deviceSet(id,false);};
                }
                /* Obtain power state after 5secs */
                setTimeout(async function() {
                  const resp = await fetch('/device?operation=getPowerState&id=' + id);
                  const data = await resp.json();
                  document.getElementById('devstatus_'+id).innerHTML = data.power + ' w ' + data.rssi + 'db ' + data.ip;
                }, 2000);
            } else {
            /** TODO error handling */
            }
        })
}


// ----------------------------------------------- Execute on load
getInfo('/info?what=hostname', 'hostname');
getInfo('/info?what=devices', 'devices', formatDevices);
