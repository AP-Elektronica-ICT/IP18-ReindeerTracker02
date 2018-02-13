var mongoose = require('mongoose');
var Schema = mongoose.Schema;

module.exports = mongoose.model('Log', new Schema({
    deviceKey: {
        type: String,
        required: true
    },
    lat: {
        type: Number,
        required: true
    },
    long: {
        type: Number,
        required: true
    },
    date: {
        type: Date,
        default: Date.now
    },
    battery: {
        type: Number,
        required: true
    },
    isAlive: {
        type: Boolean,
        required: true
    }
}));