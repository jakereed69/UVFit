var express = require('express');
var router = express.Router();

// Import the model for Device documents
var Device = require("../models/device");
var Activity = require("../models/activity"); 

// Function to generate a random apikey consisting of 32 characters
function getNewApikey() {
    var newApikey = "";
    var alphabet = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789";
    
    for (var i = 0; i < 32; i++) {
       newApikey += alphabet.charAt(Math.floor(Math.random() * alphabet.length));
    }

    return newApikey;
}

// GET request return one or "all" devices registered and last time of contact.
router.get('/status/:devid', function(req, res, next) {
    var deviceId = req.params.devid;
    var responseJson = { devices: [] };

    if (deviceId == "all") {
      var query = {};
    }
    else {
      var query = {
          "deviceId" : deviceId
      };
    }
    
    Device.find(query, function(err, allDevices) {
      if (err) {
        var errorMsg = {"message" : err};
        res.status(400).json(errorMsg);
      }
      else {
         for(var doc of allDevices) {
            responseJson.devices.push({ "deviceId": doc.deviceId,  "lastContact" : doc.lastContact});
         }
      }
      res.status(200).json(responseJson);
    });
});

router.post('/register', function(req, res, next) {
    var responseJson = {
        registered: false,
        message : "",
        apikey : "none"
    };
    var deviceExists = false;

    // Ensure the request includes the deviceId parameter
    if( !req.body.hasOwnProperty("deviceId")) {
        responseJson.message = "Missing deviceId.";
        res.status(400).json(responseJson);
        return;
    }
   
    // Ensure the request includes the email parameter
    if( !req.body.hasOwnProperty("email")) {
        responseJson.message = "Missing email address.";
        res.status(400).json(responseJson);
        return;
    }

    // See if device is already registered
    Device.findOne({ deviceId: req.body.deviceId }, function(err, device) {
        if (device !== null) {
            responseJson.message = "Device ID " + req.body.deviceId + " already registered.";
            res.status(400).json(responseJson);
        }
        else {
            // Get a new apikey
	         deviceApikey = getNewApikey();
	         
	         // Create a new device with specified id, user email, and randomly generated apikey.
            var newDevice = new Device({
                deviceId: req.body.deviceId,
                userEmail: req.body.email,
                apikey: deviceApikey
            });

            // Save device. If successful, return success. If not, return error message.
            newDevice.save(function(err, newDevice) {
                if (err) {
                    console.log("Error: " + err);
                    responseJson.message = err;
                    // This following is equivalent to:
                    //     res.status(400).send(JSON.stringify(responseJson));
                    res.status(400).json(responseJson);
                }
                else {
                    responseJson.registered = true;
                    responseJson.apikey = deviceApikey;
                    responseJson.message = "Device ID " + req.body.deviceId + " was registered.";
                    res.status(201).json(responseJson);
                }
            });
        }
    });
});

module.exports = router;
