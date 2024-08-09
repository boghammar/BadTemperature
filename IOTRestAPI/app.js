/*

References:
- dotenv: https://medium.com/@akhilanand.ak01/simplify-your-node-js-configuration-with-dotenv-env-ee371ad6bf9a
- nodejs & Firebase FileStore: https://dev.to/ibukunfolay/build-a-nodejs-server-using-firebasefirestore-crud-2725
- nodejs & Firebase RTDB https://medium.com/@rajeev.sharma1804/the-power-of-node-js-and-firebase-real-time-applications-with-email-notifications-62626306499d

*/

// Firebase URL https://iotrestapi-default-rtdb.europe-west1.firebasedatabase.app/
// 

const express = require('express');
const app = express();
const config = require('./config');
const PORT = process.env.PORT || 3000

const database = require('./firebase');
const routes = require('./routes');

app.use('/api', routes);

app.listen(PORT, '0.0.0.0', () => {
    console.log("Server Listening on PORT:", PORT);
});

app.get('/', (request, response) => {
    console.log("Root (/) called");
    response.json({'Hello': 'World'});
});
