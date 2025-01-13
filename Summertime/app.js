var express = require('express');
var path = require('path');
var cookieParser = require('cookie-parser');
var logger = require('morgan');

var indexRouter = require('./routes/index');
var apiRouter = require('./routes/api');
const os = require('os');
const {version} = require('./package.json');

var app = express();

app.use(logger('dev'));
app.use(express.json());
app.use(express.urlencoded({ extended: false }));
app.use(cookieParser());
app.use(express.static(path.join(__dirname, 'public')));

app.use('/', indexRouter);
app.use('/api', apiRouter);

app.get('/info', handleInfo);

async function handleInfo(request, response) {
  console.log("/info Request for " + request.url + " from " + request.socket.remoteAddress + ":" + request.socket.remotePort);
  var ret = "";
  const q = request.query.what;
  switch (q) {
    case "hostname": ret = os.hostname(); break
    case "version": ret = version; break
    default: ret = "Unknown what Parameter '" + q + "'";
  }
  response.send(ret)
};

module.exports = app;
