const express = require('express');
const router = express.Router();
const { modelInstance } = require('./models');

// ------------------------------------------------------------------------------
// Middleware
// ------------------------------------------------------------------------------
const apiAccessLog = (req, res, next) => {
    console.log(req.method, req.url);
    next(); 
}

router.use(apiAccessLog);

router.use((err, req, res, next) => {
    console.log(err);
    next();
})

// ------------------------------------------------------------------------------
//                      API Routes
// ------------------------------------------------------------------------------
router.get('/', (req, res) => {
    res.send('Hello API!');
});
// ------------------------------------------------------------------------------
// /device/:id/{operation}
//  id: the id of the device
//  operation: status, refresh, turnon, turnoff
// ------------------------------------------------------------------------------
router.get('/device/:id/:operation?', async (req, res) => {
    const op = req.params.operation || 'status';
    try {
        console.log(`In APIROUTES - Model name = ${modelInstance.name}`);
        const resp = await modelInstance.operateOnDevice(req.params.id, op);
        res.json(resp);
    } catch (error) {    
        console.error(error);
        res.status(500).json({error: error});
    }
});

module.exports = router;