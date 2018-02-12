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
    var saved = [];
    for (var  i = 0; i < deviceKeys.length; i++) {
        const device = new Device({
            deviceKey:deviceKeys[i]
        });
        device.save()
            .then(function () {
                saved.push(device.deviceKey);
                devicePostCallback(deviceKeys, keyErrors, saved, res);
            }).catch(function (err) {
                keyErrors.push(device.deviceKey);
                devicePostCallback(deviceKeys, keyErrors, saved, res);
            })
    }
});

function devicePostCallback(deviceKeys, keyErrors, saved, res) {
    if (keyErrors.length + saved.length == deviceKeys.length) {
        if (keyErrors.length > 0) {
            res.status(500);
        } else {
            res.status(200);
        }
        res.json({
            added: saved,
            errors: keyErrors
        })
    }
}

router.post('/device/single', function (req, res) {
    console.log('body: ' + req.body);
    const device = new Device({
        deviceKey:req.body
    });
    device.save()
        .then(function () {
            res.status(201).send('device added');
        })
        .catch(function (err) {
            res.status(500).send('could not add device')
        })
})

module.exports = router;