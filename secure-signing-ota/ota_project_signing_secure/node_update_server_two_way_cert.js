const fs = require('fs');
const https = require('https');
const express = require('express');
const path = require('path');

const app = express();

var PORT = 8080;
  
// Without middleware
app.get('/firmware.bin.signed', function(req, res, next){
    if (!req.client.authorized) {
        console.log('Client can not be authenticated');
        return res.status(401).send('Invalid client certificate authentication.');
    }
    var options = {
        root: path.join(__dirname)
    };
      
    var fileName = 'firmware.bin.signed';

    res.sendFile(fileName, options, function (err) {
        if (err) {
            next(err);
        } else {
            console.log('Sent:', fileName);
        }
    });
});
  
https
  .createServer(
    {
      // ...
      requestCert: true,
      rejectUnauthorized: false,
      cert: fs.readFileSync('../../tls-mqtt/server/server.crt'),
      key: fs.readFileSync('../../tls-mqtt/server/server.key'),
      ca: fs.readFileSync('../../tls-mqtt/ca.crt'),
      // ...
    },
    app
  )
  .listen(PORT);

