var mongoose = require('mongoose');
var Schema = mongoose.Schema;

module.exports = mongoose.model('Device', new Schema({
    deviceKey: {
        type: String,
        required: true,
        unique: true
    },
    userID: String,
    activated: {
        type: Boolean,
        default: false
    },
    name: String,
    logs: [{
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
    }]
}));