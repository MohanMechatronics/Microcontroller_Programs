#include <Arduino.h>
#include <WiFi.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <ESP32Servo.h>
#include <sstream>

#define PAN_PIN 14
#define TILT_PIN 27

#define LINK1 26
#define LINK2 25
#define LINK3 18
#define GRIPPER 19

Servo panServo;
Servo tiltServo;
Servo link1Servo;
Servo link2Servo;
Servo link3Servo;
Servo GripperServo;


/* Wifi Crdentials */
const char* ssid = "Mr.BGM";
const char* password = "gunasree";

AsyncWebServer server(80);
AsyncWebSocket wsServoInput("/ServoInput");

const char* htmlHomePage PROGMEM = R"HTMLHOMEPAGE(
<!DOCTYPE html>
<html>
  <head>
  <meta name="viewport" content="width=device-width, initial-scale=1, maximum-scale=1, user-scalable=no">
    <style>
    .noselect {
      -webkit-touch-callout: none; /* iOS Safari */
        -webkit-user-select: none; /* Safari */
         -khtml-user-select: none; /* Konqueror HTML */
           -moz-user-select: none; /* Firefox */
            -ms-user-select: none; /* Internet Explorer/Edge */
                user-select: none; /* Non-prefixed version, currently
                                      supported by Chrome and Opera */
                            
    }
    
    
    .sidecontainer {
      width: 100%;
      margin-left: -50px;
      padding: 0;
    }
    .slider {
    -webkit-appearance: none;
      width: 20%;
      height: 20px;
      border-radius: 5px;
      background: #d3d3d3;
      outline: none;
      opacity: 0.7;
      -webkit-transition: .2s;
      transition: opacity .2s;

    }
    .slider:hover {
      opacity: 1;
    }
  
    .slider::-webkit-slider-thumb {
      -webkit-appearance: none;
      appearance: none;
      width: 40px;
      height: 40px;
      border-radius: 50%;
      background: red;
      cursor: pointer;
    }
    .slider::-moz-range-thumb {
      width: 40px;
      height: 40px;
      border-radius: 50%;
      background: red;
      cursor: pointer;
    }
    .sliders{
        width: 100%;
        height: 200px;
        display: flex;
        flex-direction: column;
        justify-content: space-evenly;
    }

    @media only screen and (max-width: 600px) {
  .slider {
   width: 200px;
  }
}

    </style>
  
  </head>
  <body class="noselect" align="center" style="background-color:white">
     
    <!--h2 style="color: teal;text-align:center;">Articulated Arm &#128663; Control</h2-->
    
</br></br>
</br></br>
<DIV class="sliders">
</br></br>
<div>
  <span>PAN :</span>     
  <input type="range" min="0" max="180" value="90" class="slider" id="Pan" oninput='Pan_value(this.value),sendButtonInput("Pan",value)'>
<span id="PanValue" style="color:red;font-weight:bold;">
</div>
</br>
</br>
</br>
<div>
  <span>TILT :</span>
  <input type="range" min="0" max="180" value="85" class="slider" id="Tilt" oninput='Tilt_value(this.value),sendButtonInput("Tilt",value)'>
<span id="TiltValue" style="color:red;font-weight:bold;">
</div>
</br>
</br>
</br>
<div>
  <span>Link1 :</span>
  <input type="range" min="0" max="180" value="157" class="slider" id="Link1" oninput='Link1_value(this.value),sendButtonInput("Link1",value)'>
<span id="Link1Value" style="color:red;font-weight:bold;">
</div>
</br>
</br>
</br>
<div >
  <span>Link2 :</span>
  <input type="range" min="0" max="180" value="156" class="slider" id="Link2" oninput='Link2_value(this.value),sendButtonInput("Link2",value)'>
<span id="Link2Value" style="color:red;font-weight:bold;">
</div>
</br>
</br>
</br>
<div >
  <span>Link3 :</span>
  <input type="range" min="0" max="180" value="90" class="slider" id="Link3" oninput='Link3_value(this.value),sendButtonInput("Link3",value)'>
<span id="Link3Value" style="color:red;font-weight:bold;">
</div>
</br>
</br>
</br>
<div >
  <span>EOAT :</span>
  <input type="range" min="0" max="180" value="90" class="slider" id="Gripper" oninput='Gripper_value(this.value),sendButtonInput("Gripper",value)'>
<span id="GripperValue" style="color:red;font-weight:bold;">
</div>
</DIV>
    <script>
      var webSocketServoInputUrl = "ws:\/\/" + window.location.hostname + "/ServoInput";      
      var websocketServoInput;
      
      
      function initServoInputWebSocket() 
      {
        websocketServoInput = new WebSocket(webSocketServoInputUrl);
        websocketServoInput.onopen    = function(event)
        {
          var panButton = document.getElementById("Pan");
          sendButtonInput("Pan", panButton.value);
          var tiltButton = document.getElementById("Tilt");
          sendButtonInput("Tilt", tiltButton.value);
          var link1Button = document.getElementById("Link1");
          sendButtonInput("Link1", link1Button.value);
          var link2Button = document.getElementById("Link2");
          sendButtonInput("Link2", link2Button.value);
          var link3Button = document.getElementById("Link3");
          sendButtonInput("Link3", link3Button.value);
          var GripperButton = document.getElementById("Gripper");
          sendButtonInput("Gripper", GripperButton.value);
                    
        };
        websocketServoInput.onclose   = function(event){setTimeout(initServoInputWebSocket, 2000);};
        websocketServoInput.onmessage = function(event){};        
      }
      function Pan_value(x)
	 {
	  document.getElementById("PanValue").innerHTML=x;
	 }
      function Tilt_value(x)
	 {
	  document.getElementById("TiltValue").innerHTML=x;
	 }
      function Link1_value(x)
	 {
	  document.getElementById("Link1Value").innerHTML=x;
	 }
      function Link2_value(x)
	 {
	  document.getElementById("Link2Value").innerHTML=x;
	 }
      function Link3_value(x)
	 {
	  document.getElementById("Link3Value").innerHTML=x;
	 }
      function Gripper_value(x)
	 {
	  document.getElementById("GripperValue").innerHTML=x;
	 }
      function initWebSocket() 
      {
        initServoInputWebSocket();
      }
      function sendButtonInput(key, value) 
      {
        var data = key + "," + value;
        websocketServoInput.send(data);
      }
    
      window.onload = initWebSocket;
      document.getElementById("mainTable").addEventListener("touchend", function(event){
        event.preventDefault()
      });      
    </script>
  </body>    
</html>
)HTMLHOMEPAGE";

void handleRoot(AsyncWebServerRequest *request) 
{
  request->send_P(200, "text/html", htmlHomePage);
}

void handleNotFound(AsyncWebServerRequest *request) 
{
    request->send(404, "text/plain", "File Not Found");
}

void onServoInputWebSocketEvent(AsyncWebSocket *server, 
                      AsyncWebSocketClient *client, 
                      AwsEventType type,
                      void *arg, 
                      uint8_t *data, 
                      size_t len) 
{                      
  switch (type) 
  {
    case WS_EVT_CONNECT:
      Serial.printf("WebSocket client #%u connected from %s\n", client->id(), client->remoteIP().toString().c_str());
      break;
    case WS_EVT_DISCONNECT:
      Serial.printf("WebSocket client #%u disconnected\n", client->id());
      panServo.write(90);
      tiltServo.write(85);
      link1Servo.write(157);
      link2Servo.write(156);
      link3Servo.write(90);
      GripperServo.write(90);
      break;
    case WS_EVT_DATA:
      AwsFrameInfo *info;
      info = (AwsFrameInfo*)arg;
      if (info->final && info->index == 0 && info->len == len && info->opcode == WS_TEXT) 
      {
        std::string myData = "";
        myData.assign((char *)data, len);
        Serial.printf("Key,Value = [%s]\n", myData.c_str());   
        std::istringstream ss(myData);
        std::string key, value;
        std::getline(ss, key, ',');
        std::getline(ss, value, ',');
        if ( value != "" )
        {
          int valueInt = atoi(value.c_str());
          if (key == "Pan")
          {
            panServo.write(valueInt);
          }
          else if (key == "Tilt")
          {
            tiltServo.write(valueInt);   
          }
          else if (key == "Link1")
          {
            link1Servo.write(valueInt);   
          }
          else if (key == "Link2")
          {
            link2Servo.write(valueInt);   
          }
          else if (key == "Link3")
          {
            link3Servo.write(valueInt);   
          }
          else if (key == "Gripper")
          {
            GripperServo.write(valueInt);   
          }        
        }
      }
      break;
    case WS_EVT_PONG:
    case WS_EVT_ERROR:
      break;
    default:
      break;  
  }
}


void setUpPinModes()
{  
  panServo.attach(PAN_PIN);
  tiltServo.attach(TILT_PIN);
  link1Servo.attach(LINK1);
  link2Servo.attach(LINK2);
  link3Servo.attach(LINK3);
  GripperServo.attach(GRIPPER);
  
}


void setup(void) 
{
  setUpPinModes();
  Serial.begin(115200);

  WiFi.softAP(ssid,password);
  IPAddress IP = WiFi.softAPIP();
  Serial.print("AP IP address: ");
  Serial.println(IP);

  server.on("/", HTTP_GET, handleRoot);
  server.onNotFound(handleNotFound);

  wsServoInput.onEvent(onServoInputWebSocketEvent);
  server.addHandler(&wsServoInput);

  server.begin();
  Serial.println("HTTP server started");

}


void loop() 
{ 
  wsServoInput.cleanupClients(); 

}
