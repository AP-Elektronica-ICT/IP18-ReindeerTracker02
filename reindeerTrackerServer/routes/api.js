var express = require('express');
var ObjectId = require('mongoose').Types.ObjectId;
var router = express.Router();
var config = require('../config');

/////////////////////////////////////////////////////////////////
// DEVICES
/////////////////////////////////////////////////////////////////

var Device = require('../models/device');
var Log = require('../models/log');

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
    Device.findOne({deviceKey: deviceKey})
        .then(function (device) {
            var returnDevice = device;
            returnDevice.logs = returnDevice.logs.slice(0,5);
            res.status(200).json(returnDevice);
        })
        .catch(function (err) {
            res.status(404).send('could not find device');
        })
});

router.get('/devices/:deviceKey/logs', function (req, res) {
    const deviceKey = req.params.deviceKey;
    var start = parseInt(req.query.start);
    const amount = parseInt(req.query.amount);
    if (isNaN(start)) {
        start = 0;
    }
    if (isNaN(amount)) {
        res.status(400).send('amount is not given, bad request');
    } else {
        Device.findOne({deviceKey: deviceKey})
            .then(function (device) {
                const logs = device.logs;
                const returnLogs = logs.slice(start, (start + amount));
                res.status(200).json(returnLogs);
            })
            .catch(function (err) {
                res.status(404).send('could not find device');
            })
    }
});

router.put('/devices/:deviceKey/logs', function (req, res) {
    console.log('put');
    const deviceKey = req.params.deviceKey;
    const log = req.body;
    Device.update(
        {deviceKey: deviceKey},
        {$push: {logs: log}}
    )
        .then(function (status) {
            res.status(200).send('log added');
        })
        .catch(function (err) {
            res.status(500).send('could not add log to device');
        })
});

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