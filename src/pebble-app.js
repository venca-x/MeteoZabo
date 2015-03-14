var xhrRequest = function (url, type, callback) {
  var xhr = new XMLHttpRequest();
  xhr.onload = function () {
    callback(this.responseText);
  };
  xhr.open(type, url);
  xhr.send();
};

function getWeather() {
  console.log("getWeather()");
  // Construct URL
  var url = "http://meteo.sdhzabori.cz/homepage/get-temp";
  //var url = "http://api.openweathermap.org/data/2.5/weather?lat=49.2&lon=14.22";

  // Send request to OpenWeatherMap
  xhrRequest(url, 'GET', 
    function(responseText) {
      
      // responseText contains a JSON object with weather info
      var json = JSON.parse(responseText);
      
      //console.log( json.main.temp );

      // Temperature out
      var temperature_out = json.temp['out'];
      console.log("Temperature out: " + temperature_out);

      // Conditions
      var temperature_in = json.temp['in'];
      console.log("Temperature in: " + temperature_in);
      
      //rain "rain":{"5m":0,"1h":0,"24h":0.3,"7d":2.4}
      var rain_5m = json.rain['5m'];
      var rain_1h = json.rain['1h'];
      var rain_24h = json.rain['24h'];
      
      console.log("Rain 5m: " + rain_5m );
      console.log("Rain 1h: " + rain_1h );
      console.log("Rain 24h: " + rain_24h );
      
      // Last update
      var pressure = json.pressure;
      console.log("Pressure: " + pressure);
      
      // Last update
      var last_update = json.date;
      console.log("Last update: " + last_update);


      // Assemble dictionary using our keys
      var dictionary = {
        "KEY_TEMPERATURE_OUT": temperature_out+"",
        "KEY_TEMPERATURE_IN": temperature_in+"",
        "KEY_LAST_UPDATE": last_update+"",
        "KEY_RAIN_5M": rain_5m+"",
        "KEY_RAIN_1H": rain_1h+"",
        "KEY_RAIN_24H": rain_24h+"",    
        "KEY_PRESSURE": pressure+"",
      };      

      // Send to Pebble
      Pebble.sendAppMessage(dictionary,
        function(e) {
          console.log("Weather info sent to Pebble successfully!");
        },
        function(e) {
          console.log("Error sending weather info to Pebble!");
        }
      );
    }      
  );
}

// Listen for when the watchface is opened
Pebble.addEventListener('ready', 
  function(e) {
    console.log("PebbleKit JS ready!");

    // Get the initial weather
    getWeather();
  }
);

// Listen for when an AppMessage is received
Pebble.addEventListener('appmessage',
  function(e) {
    console.log("AppMessage received!");
    getWeather();
  }                     
);
