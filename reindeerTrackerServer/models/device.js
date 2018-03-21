var mongoose = require('mongoose');

var deviceSchema = new mongoose.Schema({
    deviceKey: {
        type: String,
        required: true,
        unique: true
    },
    userIDs: [{
        type: String,
        unique: true
    }],
    activated: {
        type: Boolean,
        default: false
    },
    activationDate: Date,
    isAlive: {
        type: Boolean,
        default: true
    },
    name: String,
    birthyear: String,
    imageUrl: String,
    gender: String,
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
    }]},
    {
        toObject: {
            virtuals: true
        },
        toJSON: {
            virtuals: true
        }
    }
);

deviceSchema.virtual('lastLog').get(function () {
    const lastLog = this.logs[0];
    return lastLog;
});

module.exports = mongoose.model('Device', deviceSchema);