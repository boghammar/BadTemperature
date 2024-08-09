const express = require('express');
const router = express.Router();

router.use(express.json());

// Define routes
router.get('/users', (req, res) => {
  res.send('List of users');
});

router.get('/status', (request, response) => {
    console.log(formatTime() + ": /status called by " + request.ip);
    const status = {
       'Status': 'Running'
    };
    
    response.send(status);
 });

router.post('/signal', (request, response) => {
    console.log(formatTime() + ": /signal called by ", request.ip, " payload:", request.body);
    response.json({'Status': 'ok'});
});

/*router.put('/users/:id', (req, res) => {
  const userId = req.params.id;
  res.send(`Update user ${userId}`);
});

router.delete('/users/:id', (req, res) => {
  const userId = req.params.id;
  res.send(`Delete user ${userId}`);
});*/

function formatTime() {
    var d = new Date();
    return "" + d.getHours() + ":" + d.getMinutes() + ":" + d.getSeconds();
 
}

module.exports = router;