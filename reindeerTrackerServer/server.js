var express = require('express');
var mongoose = require('mongoose');
var bodyParser = require('body-parser');
var cors = require('cors');
var config = require('./config');

mongoose.connect(config.database);

var app = express();
app.use(bodyParser.urlencoded({ extended: true }));
app.use(bodyParser.json());
app.use(cors({
    origin: 'http://localhost:4200',
    credentials: true
}));

app.use('/api', require('./routes/api'));

app.listen(3000);
console.log('API is running on port 3000');