function sendIpLocationRequest() {
    var ipInput = document.getElementById("ipInput").value;
    // Sends an Ajax request to read get the exchange rate
    // from fixer.io.
    
    // 1. Create an XMLHttpRequest
    var xhr = new XMLHttpRequest();

    // 2. Assign an load event handler
    xhr.addEventListener("load", responseReceivedHandler);

    // 3. Specifiy the response type
    xhr.responseType = "json";

    // 4. Open reqeust with the type (GET) and the URL ()
    xhr.open("GET", "https://ipapi.co/" + ipInput + "/json/");

    // 5. Send it
    xhr.send();
}

function responseReceivedHandler() {
    var ipLocationElement = document.getElementById("ipLocation");

    if (this.status === 200) {
        console.log(JSON.stringify(this.response));
        ipLocationElement.value = this.response.city;
    }
    else {
        ipLocationElement.value = "Error getting location";
    }
}

document.getElementById("getLocation").addEventListener("click", sendIpLocationRequest);
