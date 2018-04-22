var mongoose = require('mongoose');

var userSchema = new mongoose.Schema({
    uid: {
      type: String,
      required: true
    },
    admin: {
        type: Boolean,
        default: false
    },
    email: {
        type: String,
        required: true
    },
    firstName: {
        type: String,
        required: true
    },
    lastName: {
        type: String,
        required: true
    },
    birthdate: {
        type: Date,
        required: true
    },
    phoneNumber: {
        type: String,
        required: true
    },
    location: {
        type: String,
        required: true
    },
    deviceToken: String,
    notifications: [{
        title: String,
        message: String,
        link: String,
        linkMessage: String,
        seen: {
            type: Boolean,
            default: false
        }
    }]
},
    {
        toObject: {
            virtuals: true
        },
        toJSON: {
            virtuals: true
        }
    });

userSchema.virtual('unseen').get(function () {
    var unseen = 0;
    for (var i=0; i<this.notifications.length; i++) {
        if (!this.notifications[i].seen) {
            unseen++;
        }
    }
    return unseen;
});

module.exports = mongoose.model('User', userSchema);