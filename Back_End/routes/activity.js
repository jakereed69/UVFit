var express = require('express');
var router = express.Router();

// Import the model for Activity documents
var Activity = require("../models/activity"); 
var Device = require("../models/device"); 


// GET request return one or "all" devices registered and last time of contact.
router.get('/summary/:devid', function(req, res, next) {
    var deviceId = req.params.devid;
    var responseJson = { activity: [] };

    if (deviceId == "all") {
      var query = {};
    }
    else {
      var query = {
          "deviceId" : deviceId
      };
    }

    Activity.find(query, function(err, allActivity) {
      if (err) {
        var errorMsg = {"message" : err};
        res.status(400).json(errorMsg);
      }
      else {
         for(var doc of allActivity) {
            responseJson.activity.push({ "deviceId": doc.deviceId,  "start" : doc.start, "stop": doc.stop, "info": doc.info});
         }
      }
      res.status(200).json(responseJson);
    });
	    
    
});

router.post('/update', function(req, res, next) { 
    var responseJson = {
        logged: false,
        message : "",
    };

    // Ensure the request includes the deviceId parameter
    if( !req.body.hasOwnProperty("deviceId")) {
        responseJson.message = "Missing deviceId.";
        res.status(400).json(responseJson);
        return;
    }
    // Ensure the request includes the latitude parameter
    if( !req.body.hasOwnProperty("start")) {
        responseJson.message = "Missing activity start time information.";
        res.status(400).json(responseJson);
        return;
    }
    // Ensure the request includes the longitude parameter
    if( !req.body.hasOwnProperty("stop")) {
        responseJson.message = "Missing activity stop time information.";
        res.status(400).json(responseJson);
        return;
    }
    // Ensure the request includes the speed parameter
    if( !req.body.hasOwnProperty("info")) {
        responseJson.message = "Missing sensor information.";
        res.status(400).json(responseJson);
        return;
    }
   
    // See if device is registered
    Device.findOne({ deviceId: req.body.deviceId }, function(err, device) {
        if (device !== null) {
	    // log this activity


	    // Create a new activity with specified id, times, and sensor info values.
            var newActivity = new Activity({
                deviceId: req.body.deviceId,
                start: req.body.start,
                stop: req.body.stop, 
		info: req.body.info
            });

 	    // Save activity. If successful, return success. If not, return error message.
            newActivity.save(function(err, newActivity) {
                if (err) {
                    console.log("Error: " + err);
                    responseJson.message = err;
                    res.status(400).json(responseJson);
                }
                else {
	   	    responseJson.logged = true; 
            	    responseJson.message = "Activity update logged";
            	    res.status(200).json(responseJson);
                }
            });
        }
        else {
            // device not registered so ignore the update

            responseJson.message = "DeviceID is not registered";
            res.status(400).json(responseJson);
        }
    });
});

module.exports = router;
