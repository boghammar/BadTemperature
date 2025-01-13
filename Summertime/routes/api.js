// Test API
// Influx v1 API https://node-influx.github.io/class/src/index.js~InfluxDB.html
var express = require('express');
var rateLimit = require('express-rate-limit');
var Joi = require('joi');
var router = express.Router();

require('dotenv').config();
let influxUser = process.env.INFLUX_USER;
let influxPassword = process.env.INFLUX_PASSWORD;
let influxHost = process.env.INFLUX_HOST;
let influxDB = process.env.INFLUX_DB;

const Influx = require('influx');
const influx = new Influx.InfluxDB('http://' + influxUser + ':' + influxPassword + '@' + influxHost + ':8086/' + influxDB);

// ---------------------------------- Limit requests max 3 in one minute
const rateLimiter = rateLimit({
  windowsMS: 1 * 60 * 1000, //15*60*1000,
  max: 3 //100
});
// padkod 9709
/* ------------------------------------------- GET sensor data */
router.get('/sensor/:site/:location/:operation', rateLimiter, function (req, res, next) {

  try {
    const schema = Joi.object({
      site: Joi.string().regex(/^[\w\d]+$/).required(), // Ensure parameters are only letters and numbers
      location: Joi.string().regex(/^[\w\d]+$/).required(),
      operation: Joi.string().regex(/^[\w\d]+$/).required()
    });

    const { error } = schema.validate(req.params);

    if (error) {
      return res.status(400).send('Invalid site: ' + error);
    }

    let time = new Date().toISOString();
    let location = req.params.site + '/' + req.params.location;

    let qq = 'select last("value") as value,timeHuman from iot where location = \'' + location + "'";
    console.log(qq);
    influx.query(qq).then(result => {
      console.log(result);
      if (result.length == 0) throw new Error('No data');
      console.log(result[0].value);
      console.log(result[0].timeHuman);
      res.json({ location: location, datatype: req.params.operation, value: result[0].value, timeHuman: result[0].timeHuman, time: time });
    }).catch(error => {
      res.status(500).json({error: 'Internal db error: '+error.message});
    });


  } catch (error) {

    // Dont send back the actual error just log it 
    res.status(500).send('Internal error');
  }
});

module.exports = router;
