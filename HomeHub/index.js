const express = require('express');
const device = require('./device');
const path = require('path');
const os = require('os');
const { get } = require('http');

const app = express();
const PORT = process.env.PORT || 8080

const devices = {
  "Devices": [
    new device("1", "Julgran", "Shelly", "192.168.20.150"),
    new device("2", "Slingor", "Shelly", "192.168.20.151"),
    new device("3", "Slingorute", "Shelly", "192.168.20.152")
  ]}

const deviceMap = devices.Devices.reduce((map, device) => {
  map[device.id] = device;
  return map;
}, {});

app.use(express.static(path.join(__dirname, "public")));

app.listen(PORT, '0.0.0.0', () => {
  debug("Server Listening on PORT:", PORT);
});



// -------------------------------------------------------------------------------------
//                   Routing
// -------------------------------------------------------------------------------------
//
app.get('/device', async (request, response) => {
  debug("Request for " + request.url + " from " + request.socket.remoteAddress + ":" + request.socket.remotePort);
  var what = request.query.operation;
  var id = request.query.id;
  switch (what) {
      // ----------------------------- Status
      case "status": {
      try {
        var ret = await deviceMap[id].getStatus();
        debug("Status: " + ret);
        response.send(ret);
      } catch (error) {
        response.status(500).send("Error getting device status: " + error);
      }
      break;}
      break;
      // ----------------------------- Refresh
      case "refresh": {
      try {
        /** TODO this has to be written, just a copy right now */
        var ret = await deviceMap[id].getStatus();
        debug("Status: " + ret);
        response.send(ret);
      } catch (error) {
        response.status(500).send("Error getting device status: " + error);
      }
      break;}

      // ----------------------------- turnon
      case "turnon": {
      try {
        var turnon = request.query.turnon;

        var ret = await deviceMap[id].turnOn(turnon);
        debug("Turnon: " + JSON.stringify(ret));
        ret.power = deviceMap[id].power;
        ret.rssi = deviceMap[id].rssi;
        ret.ip = deviceMap[id].ip;
        debug("Turnon: " + deviceMap[id].toString());
        debug("Turnon: " + JSON.stringify(ret));
        response.send(ret);
      } catch (error) {
        response.status(500).send("Error turning device on: " + error);
      }
      break;}
    default: response.send("Unknown operation '" + what + "'");
  }
});
// -------------------------------------------------------------------------------------
//
app.get('/info', (request, response) => {
  debug("Request for " + request.url + " from " + request.socket.remoteAddress + ":" + request.socket.remotePort);
  var ret = "";
  const q = request.query.what;
  switch (q) {
    case "hostname": ret = os.hostname(); break
    case "devices": ret = getListOfDevices(); break
    default: ret = "Unknown what Parameter '" + q + "'";
  }
  response.send(ret)
});

// -------------------------------------------------------------------------------------
app.get('/', (request, response) => {
  debug("Request for " + request.url + " from " + request.socket.remoteAddress + ":" + request.socket.remotePort);
  //response.render('index', { title: 'Hey', message: 'Hello there!' })
  //response.json({'Hello': 'World'});
  response.send('<h1>Hello World</h1><p>Running at ' + os.hostname() + '!</p>');
});


// var http = require('http');

// const server = http.createServer(function (req, res) {
//     debug("Request for " + req.url + " from " + req.socket.remoteAddress + ":" + req.socket.remotePort);
//     res.writeHead(200, { 'Content-Type': 'text/html' });
//     res.end('<h1>Hello World</h1><p>Running at ' + os.hostname() + '!</p>');
// })

// server.listen(8080, () => {
//     debug('Server running at http://127.0.0.1:8080/');
// });

// -------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------
function getListOfDevices() {
  var ret = JSON.stringify(devices);
  debug(ret);
  return ret;
}
// -------------------------------------------------------------------------------------
//                   Utilities
// -------------------------------------------------------------------------------------
//
function debug(message) {
  console.log(`${new Date().toLocaleTimeString()} - ${message}`);
}
