var express = require('express');
var ObjectId = require('mongoose').Types.ObjectId;
var router = express.Router();
var config = require('../config');

/////////////////////////////////////////////////////////////////
// DEVICES
/////////////////////////////////////////////////////////////////

var Device = require('../models/device');

// POST key array
router.post('/device', function (req, res) {
    const deviceKeys = req.body;
    var keyErrors = [];
    var done = false;
    for (var  i = 0; i < deviceKeys.length; i++) {
        const device = new Device({
            deviceKey:deviceKeys[i]
        });
        console.log(device);
        device.save(function (err) {
            if (err) {
                keyErrors.push(deviceKeys[i]);
                console.log(err);
            }
            if (i == deviceKeys.length && !done) {
                done = true;
                console.log('done');
                if (keyErrors.length == deviceKeys.length) {
                    res.status(500).send('could not add keys');
                } else if (keyErrors.length > 0) {
                    var message = 'keys: ';
                    for (var  j = 0; j < keyErrors.length; j++) {
                        message += keyErrors[i] + ', ';
                    }
                    message += 'could not be added';
                    res.status(500).send(message);
                } else {
                    res.status(201).send('keys added');
                }
            }
        });
    }
});

module.exports = router;