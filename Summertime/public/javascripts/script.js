// -------------------------------------------------------------------------------------
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
// ---------------------------------------- Execute this on load
function getTemperatureData(location, callback, range) {
  var url = '/api/sensor/'+location+'/temp' + (range ? '/' + range : '');
  fetch(url)
  .then(response => response.json())
  .then(data => {
    console.log(data);
    callback(data);
    return data;
  })
  .catch(error => console.error('Error:', error));
}

// ---------------------------------------- Execute this on load

getInfo('./info?what=hostname', 'hostname');
getInfo('./info?what=version', 'version');

export {getTemperatureData};