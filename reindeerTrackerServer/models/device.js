var mongoose = require('mongoose');
var Schema = mongoose.Schema;

module.exports = mongoose.model('Device', new Schema({
    deviceKey: {
        type: String,
        required: true,
        unique: true
    },
    activated: {
        type: Boolean,
        default: false
    },
    name: String
}));