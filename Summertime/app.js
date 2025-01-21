var express = require('express');
var path = require('path');
var cookieParser = require('cookie-parser');
var logger = require('morgan');

var indexRouter = require('./routes/index');
var apiRouter = require('./routes/api');
const os = require('os');
const {version} = require('./package.json');

const basepath = ''; // '/summertime'; // 
var app = express();

app.use(logger('dev'));
app.use(express.json());
app.use(express.urlencoded({ extended: false }));
app.use(cookieParser());
app.use(express.static(path.join(__dirname, 'public')));

// Middleware yo grab any basepath value
app.use(function (req, res, next) {
    const basepath = req.headers['x-base-path'] || '';
    console.log("Basepath: '" + basepath + "'");
    req.basepath = basepath;
    next();
});

//app.use(basepath, indexRouter);
app.use(basepath + '/api', apiRouter);

app.get(basepath + '/info', handleInfo);

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
