// Test API
// Influx v1 API https://node-influx.github.io/class/src/index.js~InfluxDB.html
var express = require('express');
var rateLimit = require('express-rate-limit');
var Joi = require('joi');
var router = express.Router();

// ---------------------------------- Influx config and DB connection
require('dotenv').config();
let influxUser = process.env.INFLUX_USER;
let influxPassword = process.env.INFLUX_PASSWORD;
let influxHost = process.env.INFLUX_HOST;
let influxDB = process.env.INFLUX_DB;

if (!influxUser || !influxPassword || !influxHost || !influxDB) {
  console.error('Missing INFLUX_USER, INFLUX_PASSWORD, INFLUX_HOST or INFLUX_DB environment variables.');
  process.exit(1);
}

const Influx = require('influx');
const influx = new Influx.InfluxDB('http://' + influxUser + ':' + influxPassword + '@' + influxHost + ':8086/' + influxDB);

// ---------------------------------- Limit requests max 3 in one minute
const rateLimiter = rateLimit({
  windowsMS: 1 * 60 * 1000, //15*60*1000,
  max: 10 //100
});
// padkod 9709
/* ------------------------------------------- GET sensor data */
router.get('/sensor/:site/:location/:operation/:range?', rateLimiter, function (req, res, next) {

  try {
    const schema = Joi.object({
      site: Joi.string().regex(/^[\w\d]+$/).required(), // Ensure parameters are only letters and numbers
      location: Joi.string().regex(/^[\w\d]+$/).required(),
      operation: Joi.string().regex(/^[\w\d]+$/).required(),
      range: Joi.string().regex(/^[\w\d]+$/)
    });

    const { error } = schema.validate(req.params);

    if (error) {
      return res.status(400).send('Invalid arguments: ' + error);
    }

    let time = new Date().toISOString();
    let location = req.params.site + '/' + req.params.location;

    if (req.params.range) {
      let rangeMS = 1000 * 60 * 60;
      switch (req.params.range) {
        case 'hour':
          rangeMS = 1000 * 60 * 60;
          break;
        case 'day':
          rangeMS = 1000 * 60 * 60 * 24;
          break;
        case 'week':
          rangeMS = 1000 * 60 * 60 * 24 * 7;
          break;
        case 'month':
          rangeMS = 1000 * 60 * 60 * 24 * 30;
          break;
        case 'year':
          rangeMS = 1000 * 60 * 60 * 24 * 365;
          break;
      }
      let minTime = new Date().getTime()-rangeMS; 
      let qq = 'select * from iot where location = \'' + location + "' AND time > " + minTime //+ "' ORDER BY time DESC LIMIT 10";
      console.log(qq);
      influx.query(qq).then(result => {
        //console.log(result);
        if (result.length == 0) throw new Error('No data');
        console.log("Number of datapoints: " + result.length + ' First: ' + result[0].timeHuman + ' ' + result[0].value + ' Last: ' + result[result.length-1].timeHuman + ' ' + result[result.length-1].value);
        var jsonResp = [];
        for (var i = 0; i < result.length; i++) {
          jsonResp.push({ timeHuman: result[i].timeHuman, value: result[i].value });
        }
        res.json(jsonResp);
      }).catch(error => {
        res.status(500).json({error: 'Internal db error: '+error.message});
      });
    } else {
      let qq = 'select last("value") as value,timeHuman from iot where location = \'' + location + "'";
      console.log(qq);
      influx.query(qq).then(result => {
        //console.log(result);
        if (result.length == 0) throw new Error('No data');
        console.log(result[0].timeHuman + ' ' + result[0].value);
        res.json({ location: location, datatype: req.params.operation, value: result[0].value, timeHuman: result[0].timeHuman, time: time });
      }).catch(error => {
        res.status(500).json({error: 'Internal db error: '+error.message});
      });
    }


  } catch (error) {

    // Dont send back the actual error just log it 
    res.status(500).send('Internal error');
  }
});

module.exports = router;
