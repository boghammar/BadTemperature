var http = require('http');
const os = require('os');

const server = http.createServer(function (req, res) {
    debug("Request for " + req.url + " from " + req.socket.remoteAddress + ":" + req.socket.remotePort);
    res.writeHead(200, { 'Content-Type': 'text/html' });
    res.end('<h1>Hello World</h1><p>Running at ' + os.hostname() + '!</p>');
})

server.listen(8080, () => {
    debug('Server running at http://127.0.0.1:8080/');
});

function debug(message) {
  console.log(`${new Date().toLocaleTimeString()} - ${message}`);
}