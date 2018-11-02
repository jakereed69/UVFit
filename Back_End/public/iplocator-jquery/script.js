function sendIpLocationRequest() {
   $.get('https://ipapi.co/' + $('#ipInput').val() + '/json/', {},
      function(responseData) {
         console.log(responseData);

         if (responseData.error) {
            $('#ipLocation').val(responseData.reason);
         }
         else {
            $('#ipLocation').val(responseData.city);
         }
      }, 'json').fail(function(responseObject) {
          $('#ipLocation').val("Could not contact server.");
   });
}

$('#getLocation').click(sendIpLocationRequest);