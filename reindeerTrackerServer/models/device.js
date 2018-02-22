var mongoose = require('mongoose');
var Schema = mongoose.Schema;

var deviceSchema = new mongoose.Schema({
    deviceKey: {
        type: String,
        required: true,
        unique: true
    },
    userIDs: [String],
    activated: {
        type: Boolean,
        default: false
    },
    isAlive: Boolean,
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