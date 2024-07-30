const express = require('express');

const app = express();

const PORT = process.env.PORT || 3000

app.listen(PORT, '0.0.0.0', () => {
    console.log("Server Listening on PORT:", PORT);
});

app.get('/', (request, response) => {
    console.log("Root (/) called");
    response.json({'Hello': 'World'});
});

app.use(express.json());
app.post('/signal', (request, response) => {
    console.log("/signal called by ", request.ip, " payload:", request.body);
    response.json({'Status': 'ok'});
});

app.get('/status', (request, response) => {
    console.log(formatTime() + ": /status called by " + request.ip);
    const status = {
       'Status': 'Running'
    };
    
    response.send(status);
 });

 function formatTime() {
    var d = new Date();
    return "" + d.getHours() + ":" + d.getMinutes() + ":" + d.getSeconds();
 }