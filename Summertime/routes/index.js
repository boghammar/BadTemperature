/** THIS ONE IS NOT USED ANYMORE */
var express = require('express');
var router = express.Router();

/* GET home page. */
router.get('/', function(req, res, next) {
  res.render('index', { title: 'Express' });
});

router.get('/history', function(req, res, next) {
  res.render('history', { title: 'Express' });
});

module.exports = router;
