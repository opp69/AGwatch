//  file: pebble-js-app.js
//  auth: Matthew Clark, SetPebble

// change this token for your project
var setPebbleToken = '4YDG';
var transferInProgress = false;
var apikey = '&APPID=bc2db918b30a453fbc1c3be5cb99d5b7';

Pebble.addEventListener('ready', function(e) {
  console.log('PebbleKit JS ready v1.14!');
  
  //getWeather();
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
  else if ('WX_GETWEATHER' in e.payload) {
    console.log("PebbleKit JS message is GET_WEATHER");
    getWeather();
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
    };
    req.onerror = function(e) {
      errorCallback(e);
    };
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


var xhrRequest = function (url, type, callback) {
  var xhr = new XMLHttpRequest();
  xhr.onload = function () {
    callback(this.responseText);
  };
  xhr.open(type, url);
  xhr.send();
};

function locationSuccess(pos) {
  
  var cityname = "";
  var latitude = 0;
  var temperature = 0;
  var humidity = 0;
  var condition = 0;
  var wxicon = "";
  var clouds = 0;
  var day1Time = 0;
  var day1TemperatureMin = 0;
  var day1TemperatureMax = 0;

  var day2Time = 0;
  var day2TemperatureMin = 0;
  var day2TemperatureMax = 0;
      
  //var d = new Date();
  //console.log(d.toString()+' - Position Object TimeStamp='+pos.timestamp.toString());
  
  // Construct URL
  var url = "http://api.openweathermap.org/data/2.5/weather?lat=" + pos.coords.latitude + "&lon=" + pos.coords.longitude+apikey;  
  //var   url = "http://api.openweathermap.org/data/2.5/weather?lat=48.6598&lon=2.2681";
  xhrRequest(url, 'GET', 
    function(responseText) {
      // responseText contains a JSON object with weather info
      var json = JSON.parse(responseText);
      
      //console.log(JSON.stringify(json));
      
      cityname = json.name;
      temperature = Math.round(json.main.temp*100);
      humidity = Math.round(json.main.humidity);
      condition = json.weather[0].id;      
      wxicon = json.weather[0].icon;      
      clouds = json.clouds.all;    
      latitude = Math.round(pos.coords.latitude*100);
                        
      // Construct URL forecast
      var dailyurl = "http://api.openweathermap.org/data/2.5/forecast/daily?lat=" + pos.coords.latitude + "&lon=" + pos.coords.longitude+apikey;  
      //var dailyurl = "http://api.openweathermap.org/data/2.5/forecast/daily?lat=48.6598&lon=2.2681";  
      // Send request to OpenWeatherMap
      xhrRequest(dailyurl, 'GET', 
        function(responseForecastText) {
          // responseText contains a JSON object with weather info
          var json = JSON.parse(responseForecastText);  
          day1Time = json.list[0].dt;
          //var day1Conditions = json.list[0].weather[0].id;      
          day1TemperatureMin = Math.round(json.list[0].temp.min*100);
          day1TemperatureMax = Math.round(json.list[0].temp.max*100);
    
          day2Time = json.list[1].dt;
          //var day2Conditions = json.list[1].weather[0].id;      
          day2TemperatureMin = Math.round(json.list[1].temp.min*100);
          day2TemperatureMax = Math.round(json.list[1].temp.max*100);
          
          var forecasturl = "http://api.openweathermap.org/data/2.5/forecast?lat=" + pos.coords.latitude + "&lon=" + pos.coords.longitude+apikey;  
          //var forecasturl = "http://api.openweathermap.org/data/2.5/forecast?lat=48.6598&lon=2.2681";  
          // Send request to OpenWeatherMap
          xhrRequest(forecasturl, 'GET', 
            function(responseForecastText) {
              // responseText contains a JSON object with weather info
              var json = JSON.parse(responseForecastText);  
              var H0Conditions  = json.list[0].weather[0].id;      
              var H3Conditions  = json.list[1].weather[0].id;            
              var H6Conditions  = json.list[2].weather[0].id;                  
              var H9Conditions  = json.list[3].weather[0].id;                        
              var H12Conditions = json.list[4].weather[0].id;                              
                          
               var dictionary = {   
                "WX_LATITUDE": latitude, 
                "WX_TEMP100X": temperature,
                "WX_HUMIDITY": humidity,
                "WX_ICON": wxicon,              
                "WX_CITYNAME": cityname,
                "WX_CONDITION": condition,
                "WX_CLOUDS": clouds,
                "WX_DAY1_TIME": day1Time,
                //"WX_DAY1_CONDITIONS": day1Conditions,
                "WX_DAY1_TEMP_MIN": day1TemperatureMin,
                "WX_DAY1_TEMP_MAX": day1TemperatureMax,
                "WX_DAY2_TIME": day2Time,
                //"WX_DAY2_CONDITIONS": day2Conditions,
                "WX_DAY2_TEMP_MIN": day2TemperatureMin,
                "WX_DAY2_TEMP_MAX": day2TemperatureMax,
                "WX_H0_CONDITIONS": H0Conditions,
                "WX_H3_CONDITIONS": H3Conditions,         
                "WX_H6_CONDITIONS": H6Conditions,                  
                "WX_H9_CONDITIONS": H9Conditions,                           
                "WX_H12_CONDITIONS": H12Conditions                                           
               };
              
              Pebble.sendAppMessage(dictionary,
                function(e) {
                  var d = new Date();
                  console.log(d.toString()+" --> URL "+url);
                  console.log(d.toString()+" --> WX to Pebble successfully WX! "+JSON.stringify(dictionary));
                },
                function(e) {
                  console.log("Error sending WX to Pebble!");
                }
              );
            } // responseText
          ); // xhrresquest
          
          
        } // responseText
      ); // xhrresquest
          
                  
    } // responseText
  );  // xhrrequest
    
}
  
  

function locationError(err) {
  console.log("Error requesting location for WX!");
}


function getWeather() {
  navigator.geolocation.getCurrentPosition(
    locationSuccess,
    locationError,
    {timeout: 15000, maximumAge: 30000} //, enableHighAccuracy: true}
  );
}


