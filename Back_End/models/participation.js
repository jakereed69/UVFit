var db = require("../db");

// Create a "Participation" model
var Participation = db.model("Participation", {
   email:      String,
   timestamp:  { type: Date, default: Date.now },
   ipaddress: String
});

module.exports = Participation;
