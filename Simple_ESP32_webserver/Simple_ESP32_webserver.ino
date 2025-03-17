#include "esp_wifi.h"
#include <WiFi.h>
#include <WebServer.h>

const char* ssid = "I Dont Have Internet";
const char* password = "gunasree";

const int ledPin = 4;

WebServer server(80);

static const char PROGMEM html[] = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
<meta name="viewport" content="width=device-width, initial-scale=1">
<style>
.container { display: flex; flex-direction: column; align-items: center; justify-content: center; height: 100vh; }

.switch {
  position: relative;
  display: inline-block;
  width: 60px;
  height: 34px;
}

.switch input { 
  outline: 0;
  opacity: 0;
  width: 0;
  height: 0;
}

.slider2 {
  position: absolute;
  cursor: pointer;
  top: 0;
  left: 0;
  right: 0;
  bottom: 0;
  background-color: #ccc;
  -webkit-transition: .4s;
  transition: .4s;
}

.slider2:before {
  position: absolute;
  content: "";
  height: 26px;
  width: 26px;
  left: 4px;
  bottom: 4px;
  background-color: white;
  -webkit-transition: .4s;
  transition: .4s;
}

input:checked + .slider2 {
  background-color: #2196F3;
}

input:focus + .slider2 {
  box-shadow: 0 0 1px #2196F3;
}

input:checked + .slider2:before {
  -webkit-transform: translateX(26px);
  -ms-transform: translateX(26px);
  transform: translateX(26px);
}

/* Rounded sliders */
.slider2.round {
  border-radius: 34px;
}

.slider2.round:before {
  border-radius: 50%;
}
</style>
</head>
<body>

<div class="container">
<h2>Mr.BGM</h2>
<label for="controlstate" class="switch">
  <input type="checkbox" id="controlstate" onclick="myFunction()">
  <span class="slider2 round" ></span>
</label>
</div>

<script>
function myFunction() {
  var checkBox = document.getElementById("controlstate");
  var text = document.getElementById("text");
  if (checkBox.checked == true){
    var xhr = new XMLHttpRequest();
    xhr.open('GET', '/led?state=on', true);
    xhr.send();
  } else {
     var xhr = new XMLHttpRequest();
     xhr.open('GET', '/led?state=off', true);
     xhr.send();
  }
}
</script>
</body>
</html> 
)rawliteral";

void handleRoot() {
  server.send(200, "text/html", (const char*) html);
}

void handleNotFound() {
  server.send(404, "text/plain", "404: Not found");
}

void handleLed() {
  String state = server.arg("state");
  if (state == "on") {
    digitalWrite(ledPin, HIGH);
  } else if (state == "off") {
    digitalWrite(ledPin, LOW);
  }

  server.send(200, "text/plain", state);
}

void setup() {
  Serial.begin(115200);
  pinMode(ledPin, OUTPUT);

  ////////SoftAccess Point
  WiFi.softAP(ssid,password);
  IPAddress IP = WiFi.softAPIP();
  Serial.print("AP IP address: ");
  Serial.println(IP);

  /*/////////////////////////////////////////
  
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }

  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

  *////////////////////////////////////

  server.on("/", handleRoot);
  server.on("/led", handleLed);
  server.onNotFound(handleNotFound);

  server.begin();
  Serial.println("HTTP server started");
}

void loop() {
  server.handleClient();
}

