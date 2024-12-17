const express = require('express');
const os = require('os');

const app = express();
const PORT = process.env.PORT || 8080

app.listen(PORT, '0.0.0.0', () => {
  debug("Server Listening on PORT:", PORT);
});

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

function debug(message) {
  console.log(`${new Date().toLocaleTimeString()} - ${message}`);
}