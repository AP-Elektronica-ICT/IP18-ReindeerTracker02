var express = require('express');
var mongoose = require('mongoose');
var bodyParser = require('body-parser');
var cors = require('cors');
var config = require('./config');

var options = {
    useMongoClient: true,
    autoIndex: false, // Don't build indexes
    reconnectTries: Number.MAX_VALUE, // Never stop trying to reconnect
    reconnectInterval: 500, // Reconnect every 500ms
    poolSize: 10, // Maintain up to 10 socket connections
    // If not connected, return errors immediately rather than waiting for reconnect
    bufferMaxEntries: 0
};

mongoose.connect(config.database, options);

var admin = require("firebase-admin");

var serviceAccount = require("./reindeer-tracker-firebase-adminsdk-8qjsq-22476b41d0.json");

admin.initializeApp({
    credential: admin.credential.cert(serviceAccount),
    databaseURL: "https://reindeer-tracker.firebaseio.com"
});

var app = express();
app.use(bodyParser.urlencoded({ extended: true }));
app.use(bodyParser.json());
app.use(cors({
    origin: ['http://localhost:4200', 'http://localhost:8100', 'http://81.82.11.181:3001:3001'],
    credentials: true
    }));

app.use('/api', require('./routes/api'));

app.listen(3000);
console.log('API is running on port 3000');