var express = require('express');
var ObjectId = require('mongoose').Types.ObjectId;
var router = express.Router();
var admin = require("firebase-admin");
var config = require('../config');
var geodist = require('geodist');

// IMPORTANT: LOGS ARE ADDED AT THE BEGINNING OF THE ARRAY

/////////////////////////////////////////////////////////////////
// DEVICES
/////////////////////////////////////////////////////////////////

var Device = require('../models/device');

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
            res.status(201).json('device added');
        })
        .catch(function (err) {
            res.status(500).send('could not add device')
        })
});

router.get('/devices/:deviceKey', function (req, res) {
    const deviceKey = req.params.deviceKey;
    Device.findOne({deviceKey: deviceKey}, {logs: { $slice: 5}})
        .then(function (device) {
            res.status(200).json(device);
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
        Device.findOne({deviceKey: deviceKey}, {logs: { $slice: [start, amount]}})
            .then(function (device) {
                res.status(200).json(device.logs);
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
    log.initialLog = false;
    Device.update(
        {deviceKey: deviceKey},
        {$push: {logs: { $each: [log], $position: 0}}, isAlive: log.isAlive})
        .then(function (status) {
            Device.findOne({deviceKey: deviceKey})
                .then(function (device) {
                    updateDeviceAverage(device.logs, device.average, device.deviceKey);
                    if (!log.isAlive || log.battery < 20) {
                        console.log('dead or battry low, getting deviceTokens');
                        getDeviceTokens(device.userIDs)
                            .then(function (deviceTokens) {
                                console.log('got tokens');
                                console.log(deviceTokens);
                                if (!log.isAlive) {

                                    var baseMessage = {
                                        notification: {
                                            title: device.name + ' (' + device.deviceKey + ') has died.',
                                            body: 'Check the app to find the location.'
                                        }
                                    };
                                    var dbNotification = {
                                        title: device.name + ' (' + device.deviceKey + ') has died.',
                                        message: device.name + ' has unfortunatly died, check his details to find his last location.',
                                        link: '/detail?deviceKey=' + device.deviceKey,
                                        linkMessage: 'Check device.'
                                    }
                                } else {
                                    var baseMessage = {
                                        notification: {
                                            title: device.name + ' (' + device.deviceKey + ') battery is low.',
                                            body: 'Battery at ' + log.battery + '%. Please check the device to replace the battery.'
                                        }
                                    };
                                    var dbNotification = {
                                        title: device.name + ' (' + device.deviceKey + ') battery is low.',
                                        message: device.name + "'s battery is low. Please check the device to replace the battery.",
                                        link: '/detail?deviceKey=' + device.deviceKey,
                                        linkMessage: 'Check device.'
                                    }
                                }
                                console.log('notifications set');
                                for (var i=0; i< deviceTokens.length; i++) {
                                    if (deviceTokens[i] != undefined) {
                                        var message = baseMessage;
                                        message.token = deviceTokens[i];
                                        sendNotification(message);
                                    }
                                }
                                console.log('adding notifications');
                                console.log(device.userIDs[0]);
                                for (var j=0; j<device.userIDs.length; j++) {
                                    console.log('index: ' + j);
                                    console.log(device.userIDs[j]);
                                    addNotification(device.userIDs[j], dbNotification)
                                        .then(function () {
                                            console.log('added notification to: ' + device.userIDs[j]);
                                        })
                                        .catch(function (reason) {
                                            console.log(reason);
                                        })
                                }
                                res.status(200).json('log added');
                            });
                    }
                })
        })
        .catch(function (err) {
            res.status(500).send('could not add log to device');
        })
});

router.get('/test', function (req, res) {
    var baseMessage = {
        notification: {
            title: 'battery is low.',
            body: 'Battery at ' + 50 + '%. Please check the device to replace the battery.'
        },
        token: "d0SWH8LIv80:APA91bGa-PndencaYzhvJd1gYIKIcpUsueSpCRFr82dGN8ILdSr_V_Vv_eoVZWOEeaVofStYIuvfnSuo04gccg7TAID6-KEblcMFWuQI-8j8B9sqjoc7RDAb6thubJHwiR46_ACHe7jn"
    };
    sendNotification(baseMessage);
    res.json('ok');
})

function updateDeviceAverage(logs, previousAverage, deviceKey) {
    var logAmount = logs.length;
    if (logAmount > 2) {
        var dist = geodist({lat: logs[0].lat, lon: logs[0].long}, {lat: logs[1].lat, lon: logs[1].long});
        var avg = getTotalAverage(previousAverage, logAmount - 2, dist);
        Device.update({deviceKey: deviceKey}, {average: avg});
    }
}

function getTotalAverage(previousAverage, currentOfAverage, newDistance) {
    return (previousAverage * (currentOfAverage - 1) + newDistance) / currentOfAverage;
}

function getDeviceTokens(users) {
    return new Promise(function (resolve, reject) {
        User.find({uid: {$in: users}}).select('deviceToken')
            .then(function (tokens) {
                var returnArr = [];
                for (var i=0; i<tokens.length; i++) {
                    if (tokens[i] !== '' && tokens[i]) {
                        returnArr.push(tokens[i].deviceToken);
                    }
                }
                resolve(returnArr);
            })
    });
}

function sendNotification(message) {
    admin.messaging().send(message)
        .then(function (response) {
            console.log('notification sent');
        })
        .catch(function (reason) {
            console.log(reason, 'error');
        })
}

router.get('/devices/:deviceKey/details', function (req, res) {
    const deviceKey = req.params.deviceKey;
    Device.findOne({deviceKey: deviceKey})
        .then(function (device) {
            res.json(selectDeviceInfo([device], ['name', 'birthyear', 'imageUrl', 'gender', 'activated'])[0]);
        })
        .catch(function (err) {
            res.status(404).send('Could not find device');
        })
});

router.put('/devices/:deviceKey/details', function (req, res) {
    const deviceKey = req.params.deviceKey;
    const details = req.body;
    console.log(details);
    Device.update(
        {deviceKey: deviceKey},
        {name: details.name, birthyear: details.birthyear, imageUrl: details.imageUrl, gender: details.gender}
    )
        .then(function (value) {
            if (value.n <= 0) {
                res.status(404).send('device not found')
            } else {
                res.status(200).json('data updated');
            }
        })
        .catch(function (reason) {
            res.status(500).send('could not add data');
        })
});

router.put('/devices/:deviceKey/activate', function (req, res) {
    const deviceKey = req.params.deviceKey;
    Device.update(
        {deviceKey: deviceKey},
        {activated: true, activationDate: Date.now()}
    )
        .then(function (value) {
            if (value.n <= 0) {
                res.status(404).send('device not found')
            } else {
                res.status(200).json('data updated');
            }
        })
        .catch(function (reason) {
            res.status(500).send('could not add data');
        })
});

function generateBirthDate(year, month, day) {
    if (!month) {
        month = '01'
    }
    else if (month.length < 2) {
        month = '0' + month;
    }
    if (!day) {
        day = '01';
    }
    else if (day.length < 2) {
        day = '0' + day;
    }
    var dateString = year + '-' + month + '-' + day;
    console.log(dateString);
    return new Date(dateString);
}

router.get('/testnot', function (req, res) {
    addNotification('O4ztFkRL53ZJUHrSnfoXxzH31dQ2', {
        title: 'Device invite',
        message: 'You have been invited to add ' + '12345' + ' to your devices.'
    })
        .then(function () {
            res.json('done');
        })
        .catch(function (reason) {
            res.json(reason);
        })
});

router.put('/devices/:deviceKey/invite', function (req, res) {
    const deviceKey = req.params.deviceKey;
    const email = req.body.email;
    User.findOne({email: email})
        .then(function (user) {
            const uid = user.uid;
            Device.update(
                {deviceKey: deviceKey},
                {$push: {invites: uid}}
            )
                .then(function () {
                    console.log('Invite added');
                    addNotification(uid, {
                        title: 'Device invite',
                        message: 'You have been invited to add ' + deviceKey + ' to your devices.',
                        link: '/user?deviceKey=' + deviceKey,
                        linkMessage: 'Add device to list'
                    })
                        .then(function () {
                            console.log('notification added')
                        })
                        .catch(function (reason) {
                            console.log(reason);
                        })
                    User.findOne({uid: uid})
                        .then(function (user) {
                            console.log(user.deviceToken, 'deviceToken')
                            if (user.deviceToken) {
                                var message = {
                                    notification: {
                                        title: 'Device invite.',
                                        body: 'You have been invited to add ' + deviceKey + ' to your devices.'
                                    },
                                    token: user.deviceToken
                                };
                                sendNotification(message);
                            }
                        })
                    res.status(200).json('User invited: ' + uid);
                })
                .catch(function (reason) {
                    res.status(500).json('Could not invite user');
                })
        })
        .catch(function (reason) {
            res.status(404).json('Could not find user');
        })

});

/////////////////////////////////////////////////////////////////
// USERS
/////////////////////////////////////////////////////////////////
var User = require('../models/user');

router.post('/users', function (req, res) {
    const newUser = new User({
        uid: req.body.uid,
        email: req.body.email,
        firstName: req.body.firstName,
        lastName: req.body.lastName,
        birthdate: new Date(req.body.birthdate),
        phoneNumber: req.body.phoneNumber,
        location: req.body.location
    });
    if (newUser.email.includes("reindeertracker.com")) {
        newUser.admin = true;
    }
    console.log(newUser);
    newUser.save()
        .then(function (value) {
            console.log('added');
            res.status(201).json('user added');
        })
        .catch(function (reason) {
            console.log(reason);
            res.status(500).json(reason);
        })
});

router.get('/users/:userID', function (req, res) {
    const userID = req.params.userID;
    console.log(userID);
    User.findOne({uid: userID})
        .then(function (user) {
            if (user) {
                res.status(200).json(user);
            } else {
                res.status(500).json('could not find user');
            }
        })
        .catch(function (reason) {
            res.status(500).json(reason);
        })
});

router.put('/users/:userID', function (req, res) {
    const userID = req.params.userID;
    User.update({uid: userID}, {firstName: req.body.firstName, lastName: req.body.lastName, phoneNumber: req.body.phoneNumber, location: req.body.location})
        .then(function () {
            res.json('user updated');
        })
        .catch(function (reason) {
            res.status(500).json('could not edit user');
        })
});

router.put('/users/:userID/devices', function (req, res) {
    const userID = req.params.userID;
    const deviceKey = req.body.deviceKey;
    Device.findOne({deviceKey: deviceKey})
        .then(function (device) {
             if (device.userIDs.length == 0 || device.invites.indexOf(userID) != -1) {
                 if (device.logs.length <= 0) {
                     const newLog = {
                         battery: 100,
                         isAlive: true,
                         initialLog: true
                     };
                     if (device.userIDs.length == 0) {
                         var invite = userID;
                     }
                     Device.update(
                         {deviceKey: deviceKey},
                         {$push: {userIDs: userID, logs: newLog, invites: invite}}
                     )
                         .then(function (value) {
                             res.status(200).json('device registered');
                         })
                         .catch(function (reason) {
                             res.status(500).send('could not register device');
                         });
                 }
                 else {
                     Device.update(
                         {deviceKey: deviceKey},
                         {$push: {userIDs: userID}}
                     )
                         .then(function (value) {
                             res.status(200).json('device registered');
                         })
                         .catch(function (reason) {
                             res.status(500).send('could not register device');
                         });
                 }
             }
             else if (device.invites.indexOf(userID) == -1) {
                 res.status(401).json('Not invited');
             } else {
                res.status(404).json('could not find device');
            }
        })
        .catch(function (reason) {
            res.status(404).json('could not find device');
        })
});

router.get('/users/:userID/devices', function (req, res) {
    var userID = req.params.userID;
    Device.find({userIDs: userID})
        .then(function (devices) {
            //TODO: add other fields that need to be displayed in user list
            res.json(selectDeviceInfo(devices, ['deviceKey', "isAlive", 'lastLog', 'activated', 'name', 'imageUrl', 'gender', 'average']));
        })
        .catch(function (err) {
            res.status(404).send('could not find devices');
        })
});

router.delete('/users/:userID/devices', function (req, res) {
    const userID = req.params.userID;
    const deviceKey = req.query.deviceKey;
    Device.update( {deviceKey: deviceKey}, {$pullAll: {userIDs: [userID]}})
        .then(function (value) {
            res.status(200).json('deleted');
        })
        .catch(function (reason) {
            res.status(500).send('could not delete');
        })
});

router.put('/users/:userID/devicetoken', function (req, res) {
    const userID = req.params.userID;
    const deviceToken = req.body.deviceToken;
    User.update( {deviceToken: deviceToken})
        .then(function (value) {
            res.status(200).json('Token saved');
        })
        .catch(function (reason) {
            res.status(500).send('could not save token');
        })
});

router.delete('/users/:userID/devicetoken', function (req, res) {
    const userID = req.params.userID;
    User.update( {deviceToken: null})
        .then(function (value) {
            res.status(200).json('Token removed');
        })
        .catch(function (reason) {
            res.status(500).send('could not remove token');
        })
});

router.get('/users/:userID/devicetoken', function (req, res) {
    const userID = req.params.userID;
    User.findOne({uid: userID})
        .then(function (user) {
            res.status(200).json(user.deviceToken);
        })
        .catch(function (reason) {
            res.status(500).json('could not find user');
        })
});

function selectDeviceInfo(devices, keys) {
    const returnDevices = [];
    for (var  i = 0; i < devices.length; i++) {
        var newObject = {};
        for (var  j = 0; j < keys.length; j++) {
            newObject[keys[j]] = devices[i][keys[j]];
        }
        returnDevices.push(newObject);
    }
    return returnDevices;
}

router.get('/users/:userID/notifications', function (req, res) {
    const userID = req.params.userID;
    User.findOne({uid: userID})
        .then(function (user) {
            res.status(200).json(user.notifications);
        })
        .catch(function (reason) {
            res.status(500).json('could not find user');
        })
});

router.put('/users/:userID/notifications', function (req, res) {
    const userID = req.params.userID;
    addNotification(userID, req.body)
        .then(function () {
            res.status(200).json('notificaion added');
        })
});

function addNotification(userId, notification) {
    console.log('adding notification to: ' + userId, 'addNot function');
    return User.update(
        {uid: userId},
        {$push: {notifications: {$each: [notification], $position: 0}}}
    );
}

router.put('/users/:userID/notifications/seen', function (req, res) {
    const userID = req.params.userID;
    const notificationIDs = req.body;
    console.log(notificationIDs);
    for (var i=0; i<notificationIDs.length; i++) {
        setNotificationAsSeen(notificationIDs[i])
            .catch(function (reason) {
                res.status(500).json('could not mark notification as seen');
                i=notificationIDs.length;
            });
        if (i >= notificationIDs.length -1) {
            res.status(200).json('notification marked as seen');
        }
    }
});

function setNotificationAsSeen(id) {
    return User.update({'notifications._id': new ObjectId(id)}, {'$set': {
            'notifications.$.seen': true
        }})
}

router.delete('/users/:userID/notifications/:notificationID', function (req, res) {
    const userID = req.params.userID;
    const notificationID = req.params.notificationID;
    User.update({ uid: userID }, { "$pull": { "notifications": { "_id": new ObjectId(notificationID) } }}, { safe: true, multi:true })
        .then(function () {
            res.status(200).json('notification removed');
        })
        .catch(function (reason) {
            res.status(500).json('could not remove notification');
        })
})

module.exports = router;