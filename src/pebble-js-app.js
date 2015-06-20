//  file: pebble-js-app.js
//  auth: Matthew Clark, SetPebble

// change this token for your project
var setPebbleToken = '4YDG';
var transferInProgress = false;

Pebble.addEventListener('ready', function(e) {
  console.log('PebbleKit JS ready!');
});

Pebble.addEventListener('appmessage', function(e) {  
  
  if ('NETDL_URL' in e.payload) {
     console.log("PebbleKit JS message is NETDL_URL");
    
    if (transferInProgress == false) {   
       transferInProgress = true;
      console.log("Starting download");
  
      downloadBinaryResource(e.payload['NETDL_URL'], function(bytes) {
        transferImageBytes(bytes, e.payload['NETDL_CHUNK_SIZE'],
          function() { console.log("Done!"); transferInProgress = false; },
          function(e) { console.log("Failed! " + e); transferInProgress = false; }
        );
      },
      function(e) {
        console.log("Download failed: " + e); transferInProgress = false;
      });
    }
    else {
      console.log("Ignoring request to download " + e.payload['NETDL_URL'] + " because another download is in progress.");
    }
  }  
  else {
    console.log("PebbleKit JS got a not handled appmessage: " + JSON.stringify(e.payload));    
  }
    //return;
  
    
  /*
  key = e.payload.action;
  if (typeof(key) != 'undefined') {
    var settings = localStorage.getItem(setPebbleToken);
    if (typeof(settings) == 'string') {
      try {
        console.log('  +appmessage. Getting from localstorage');
        console.log('  +appmessage. sendAppMessage=%s', settings);
        Pebble.sendAppMessage(JSON.parse(settings));
      } 
      catch (e) {
      }
    }
    var request = new XMLHttpRequest();
    console.log('  +appmessage. Getting from SetPebble');
    request.open('GET', 'http://x.SetPebble.com/api/' + setPebbleToken + '/' + Pebble.getAccountToken(), true);
    request.onload = function(e) {
      if (request.readyState == 4)
        if (request.status == 200)
          try {
            console.log('  +appmessage. sendAppMessage='+request.responseText);
            Pebble.sendAppMessage(JSON.parse(request.responseText));
          } 
          catch (e) {
          }
    };
    request.send(null);
  }*/
});

Pebble.addEventListener('showConfiguration', function(e) {
  console.log('PebbleKit JS showConfiguration!');
  Pebble.openURL('http://x.SetPebble.com/' + setPebbleToken + '/' + Pebble.getAccountToken());
});

Pebble.addEventListener('webviewclosed', function(e) {
  console.log('PebbleKit JS webviewclosed!');
  if ((typeof(e.response) == 'string') && (e.response.length > 0)) {
    try {
      
      //console.log('   +webviewclosed. sendAppMessage '+e.response);     
      var configuration = JSON.parse(decodeURIComponent(e.response));
      console.log('   +webviewclosed. sendAppMessage JSON='+JSON.stringify(configuration));   
      Pebble.sendAppMessage(configuration);
   
      localStorage.setItem(setPebbleToken, e.response);      
    } 
    catch(e) {
      console.log('   +webviewclosed. CATCH');     
    }
  }
});



function downloadBinaryResource(imageURL, callback, errorCallback) {
  console.log("Downloading from "+imageURL);  
  var req = new XMLHttpRequest();
    req.open("GET", imageURL,true);
    req.responseType = "arraybuffer";
    req.onload = function(e) {
        console.log("loaded");
        var buf = req.response;
        if(req.status == 200 && buf) {
            var byteArray = new Uint8Array(buf);
            var arr = [];
            for(var i=0; i<byteArray.byteLength; i++) {
                arr.push(byteArray[i]);
            }

            console.log("Downloaded file with " + byteArray.length + " bytes.");
            callback(arr);
        }
        else {
          errorCallback("Request status is " + req.status);
        }
    }
    req.onerror = function(e) {
      errorCallback(e);
    }
    req.send(null);
}

function transferImageBytes(bytes, chunkSize, successCb, failureCb) {
  var retries = 0;

  success = function() {
    console.log("Success cb=" + successCb);
    if (successCb != undefined) {
      successCb();
    }
  };
  failure = function(e) {
    console.log("Failure cb=" + failureCb);
    if (failureCb != undefined) {
      failureCb(e);
    }
  };

  // This function sends chunks of data.
  sendChunk = function(start) {
    var txbuf = bytes.slice(start, start + chunkSize);
    console.log("Sending " + txbuf.length + " bytes - starting at offset " + start);
    Pebble.sendAppMessage({ "NETDL_DATA": txbuf },
      function(e) {
        // If there is more data to send - send it.
        if (bytes.length > start + chunkSize) {
          sendChunk(start + chunkSize);
        }
        // Otherwise we are done sending. Send closing message.
        else {
          Pebble.sendAppMessage({"NETDL_END": "done" }, success, failure);
        }
      },
      // Failed to send message - Retry a few times.
      function (e) {
        if (retries++ < 3) {
          console.log("Got a nack for chunk #" + start + " - Retry...");
          sendChunk(start);
        }
        else {
          failure(e);
        }
      }
    );
  };

  // Let the pebble app know how much data we want to send.
  Pebble.sendAppMessage({"NETDL_BEGIN": bytes.length },
    function (e) {
      // success - start sending
      sendChunk(0);
    }, failure);

}

