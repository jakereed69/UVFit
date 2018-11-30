var db = require("../db");

var activitySchema = new db.Schema({
    deviceId:     String,
    long:    	  Number,
    lat: 	  Number, 
    speed:        Number,
    uv:           Number
});

var Activity = db.model("Activity", activitySchema);

module.exports = Activity;
