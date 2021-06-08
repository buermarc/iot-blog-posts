const fs = require('fs');
const https = require('https');
const express = require('express');
const path = require('path');

const app = express();

var PORT = 8080;
  
// Without middleware
app.get('/firmware.bin.signed', function(_req, res, next){
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
      cert: fs.readFileSync('../../tls-mqtt/server/server.crt'),
      key: fs.readFileSync('../../tls-mqtt/server/server.key'),
      // ...
    },
    app
  )
  .listen(PORT);

