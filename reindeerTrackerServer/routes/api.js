var express = require('express');
var ObjectId = require('mongoose').Types.ObjectId;
var router = express.Router();
var config = require('../config');

/////////////////////////////////////////////////////////////////
// DEVICES
/////////////////////////////////////////////////////////////////

var Device = require('../models/device');

// POST key array
router.post('/devices', function (req, res) {
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
                checkAllDevicesSaved(deviceKeys, keyErrors, saved, res);
            }).catch(function (err) {
                keyErrors.push(device.deviceKey);
                checkAllDevicesSaved(deviceKeys, keyErrors, saved, res);
            })
    }
});

function checkAllDevicesSaved(deviceKeys, keyErrors, saved, res) {
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

router.post('/devices/single', function (req, res) {
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
});

router.get('/devices/:deviceKey', function (req, res) {
    const deviceKey = req.params.deviceKey;
    Device.findOne({deviceKey: deviceKey}, function (err, device) {
        if (err) {
            res.status(404).send('Device not found');
        } else {
            res.status(200).json(device);
        }
    })
})

/////////////////////////////////////////////////////////////////
// USERS
/////////////////////////////////////////////////////////////////

router.get('/users/:userID/devices', function (req, res) {
    const userID = req.params.userID;
    Device.find({userID: userID}, function (err, devices) {
        if (err) {
            res.status(404).send('could not find devices');
        } else {
            res.status(200).json(devices);
        }
    });
});

module.exports = router;