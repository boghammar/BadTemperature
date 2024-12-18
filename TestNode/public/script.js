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
        if (devices.Devices[i].output) btnclass = 'btn btn-on';
        devices.Devices[i] = 
          '<div class="deviceCard">' 
        + '<div class="deviceCardInner">'
        + '  <div class="deviceRows">'
        + '    <div>' + devices.Devices[i].name + ' ('+ devices.Devices[i].type +') </div>'
        + '    <div class="deviceStatus">' + devices.Devices[i].power + ' w ' 
        +        devices.Devices[i].rssi + 'db '
        +        devices.Devices[i].ip + ' '
        + '    </div>'
        + '  </div>'
        + '  <button class="'+ btnclass +'" id="devbtn_'+devices.Devices[i].id+'" onclick="getDeviceStatus('+devices.Devices[i].id+')"></button>'
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


// ----------------------------------------------- Execute on load
getInfo('/info?what=hostname', 'hostname');
getInfo('/info?what=devices', 'devices', formatDevices);
