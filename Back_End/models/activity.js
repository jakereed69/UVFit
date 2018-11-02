var db = require("../db");

var activitySchema = new db.Schema({
    deviceId:     String,
    start:    	  Number,
    stop: 	  Number, 
    info:         { latitude:	Number, 
		     longitude:	Number,
		     speed:	Number, 
		     uv: 	Number 
		  }
});

var Activity = db.model("Activity", activitySchema);

module.exports = Activity;
