// Pin definitions
#define RELAY_PIN1  18       // Relay 1 control pin
#define RELAY_PIN2  19       // Relay 2 control pin
#define CONTROL_SWITCH  15   // Control switch pin red
#define LIMIT_SWITCH1  4     // Limit switch 1 pin (Close) black
#define LIMIT_SWITCH2  5    // Limit switch 2 pin (Open) yellow
// ground white

// Variables to track state
bool controlState = false;   // Toggle state for the control switch
bool relay1Active = false;   // Indicates if relay 1 is active
bool relay2Active = false;   // Indicates if relay 2 is active

#include <Arduino.h>
#include <WiFi.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>

#define UP 1
#define DOWN 2
#define TURN_LEFT 3
#define TURN_RIGHT 4
#define OPEN 6
#define CLOSE 5
#define STOP 0

#define LEFT_MOTOR 0
#define RIGHT_MOTOR 1

#define FORWARD 1
#define BACKWARD -1

struct MOTOR_PINS
{
  int pinIN1;
  int pinIN2;    
};

std::vector<MOTOR_PINS> motorPins = 
{
  {26, 25},  //LEFT_MOTOR
  {14, 27},  //RIGHT_MOTOR 
};

const char* ssid     = "SIMPLE_BOT";
const char* password     = "mechatronics";

AsyncWebServer server(80);
AsyncWebSocket ws("/ws");


const char* htmlHomePage PROGMEM = R"HTMLHOMEPAGE(
<!DOCTYPE html>
<html>
    <head>
        <meta charset="utf-8">
        <meta name="viewport" content="width=device-width,initial-scale=1">
        <title>HAVE FUN WITH YOUR CONTROLS </title>
        <style>
:root {
--clr-neon: hsl(242 100% 54%);
--clr-bg: hsl(17 100% 54%);
}
            .btn{
    width: 100px;
    height: 65px;
    width:65px;
    position: relative;
    overflow: hidden;
    cursor: pointer;
    border: var(--clr-neon) 3px solid;
   background-color:transparent;
   border-radius: 0.50em;
   box-shadow: inset 0 0 10px var(--clr-neon), 0 0 30px var(--clr-neon);
   transition:all 0.5s;
}
.btn1:before {
    content: "";
    position: absolute;
    top: 0;
    left: 0;
    width: 0%;
    height: 100%;
    transition: all 0.3s;
    z-index: -1;
}
.btn2:before {
    content: "";
    position: absolute;
    top: 0;
    right: 0;
    width: 0%;
    height: 100%;
    transition: all 0.3s;
    z-index: -1;
}
.btn3:before {
    content: "";
    position: absolute;
    top: 0;
    right: 0;
    width: 100%;
    height: 0%;
    transition: all 0.3s;
    z-index: -1;
}
.btn4:before {
    content: "";
    position: absolute;
    bottom: 0;
    right: 0;
    width: 100%;
    height: 0%;
    transition: all 0.3s;
    z-index: -1;
}
@media (hover: hover){
 .btn1:hover:before{
    background-color:var(--clr-bg);
    color:#fff; 
    width: 100%;
 }
 .btn2:hover:before{
    background-color:var(--clr-bg);
    color:#fff; 
    width: 100%;
 }
 .btn3:hover:before{
    background-color:var(--clr-bg);
    color:#fff; 
    height: 100%;
 }
 .btn4:hover:before{
    background-color:var(--clr-bg);
    color:#fff; 
    height: 100%;
 }
}
@media (hover: none){
 .btn1:active:before{
    background-color:var(--clr-bg);
    color:#fff; 
    width: 100%;
 }
 .btn2:active:before{
    background-color:var(--clr-bg);
    color:#fff; 
    width: 100%;
 }
 .btn3:active:before{
    background-color:var(--clr-bg);
    color:#fff; 
    height: 100%;
 }
 .btn4:active:before{
    background-color:var(--clr-bg);
    color:#fff; 
    height: 100%;
 }
}

.btn5:before {
    content: "";
    bottom: 0;
    right: 0;
    width: 100%;
    height: 0%;
    position: absolute;
   z-index: -1;
}

.btn5:hover:before{
    background-color:var(--clr-bg);
    color:#fff; 
    height: 100%;
}
            .slider {appearance: none;width: 70%;height: 15px;border-radius: 10px;background: #ffffff;outline: none;opacity: 0.7;-webkit-transition: .2s;transition: opacity .2s;}
            .slider::-webkit-slider-thumb {appearance: none;width: 30px;height: 30px;border-radius: 50%;background: var(--clr-neon);cursor: pointer;}
            .slider:hover {opacity: 1;}
            .label {font-family: "Balsamiq Sans", cursive;color: var(--clr-neon);text-shadow: 0 0 0.25em hsl(0 0% 100% / 0.3), 0 0 0.95em currentColor;font-size: 20px;}
            /* ----------------------------------- Stream Viewer */
            img {
                width: 250;
                max-width: 100% ;
                height: 250 ; 
            }
            /* ----------------------------------- */
.triangle-up::after {
            content:"";
  border-left: 25px solid transparent;
  border-right: 25px solid transparent;
  border-bottom: 50px solid #ffffff;
           display: inline-block;
          
}
.triangle-left::after {
            content:"";
  border-top: 25px solid transparent;
  border-right: 50px solid #ffffff;
  border-bottom: 25px solid transparent;
           display: inline-block;
}
.triangle-right::after{
            content:"";
  border-top: 25px solid transparent;
  border-left: 50px solid #ffffff;
  border-bottom: 25px solid transparent;
  display: inline-block;
}
.triangle-down::after {
             content:"";
  border-left: 25px solid transparent;
  border-right: 25px solid transparent;
  border-top: 50px solid #ffffff;
           display: inline-block;
}
.stop::after {
  content:"";
  border-radius: 25px;
  border: 4px solid #73AD21;
 background: #ff1900;
  padding: 18px; 
  width: 6px;
  height: 7px;
 display: inline-block;
font-size:25px; 
color: var(--clr-neon);
position: relative;
}
.switch {
  position: relative;
  display: flex;
  border: var(--clr-neon) 3px solid;
   background-color:transparent;
   border-radius: 30px;
   box-shadow: inset 0 0 50px var(--clr-neon), 0 0 30px var(--clr-neon);
   transition:all 0.5s;
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
  background-color: var(--clr-neon);
}

input:focus + .slider2 {
  box-shadow: 0 0 5px #2196F3;
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
        /* ----------------------------------- */
        </style>
    </head>
    <body style="background-color:#000000;" class="noselect" align="center">
    <h2 style="color: rgb(255, 255, 255); text-align: center">BOT</h2>
    <h2 style="color: rgb(255, 255, 255); text-align: center">
      MECHATRONICS
    </h2>
    <br/>
    <div align=center> 
    <button class=" btn btn4 triangle-up" id="forward" ontouchstart='onTouchStartAndEnd("1")'
            ontouchend='onTouchStartAndEnd("0")'></button>
    </div>
    <br/>

    <div align=center> 
    <button class="btn btn2 triangle-left" id="turnleft" ontouchstart='onTouchStartAndEnd("3")'
            ontouchend='onTouchStartAndEnd("0")'></button>
    <button class="btn btn5 stop" id="stop"></button>
    <button class="btn btn1 triangle-right" id="turnright" ontouchstart='onTouchStartAndEnd("4")'
            ontouchend='onTouchStartAndEnd("0")'></button>
    </div>
    <br/>

    <div align=center> 
    <button class="btn btn3 triangle-down" id="backward" ontouchstart='onTouchStartAndEnd("2")'
            ontouchend='onTouchStartAndEnd("0")'></button>
    </div>
    <br/>
    <br/>
    <div align=center>
    <label for="controlstate" class="switch">
      <input type="checkbox" id="controlstate" onclick="myFunction()">
      <span class="slider2 round" ></span>
    </label>
    </div>
</br>

    <script>
      function myFunction() {
        var checkBox = document.getElementById("controlstate");
        var text = document.getElementById("text");
        if (checkBox.checked == true){
          websocket.send(5);
          console.log("ok");
        } else {
          websocket.send(6);
          console.log("off");
        }
      }
      function stop() {
        document.body.style.backgroundColor = "red";

        setTimeout(function () {
          document.body.style.backgroundColor = "";
        }, 1000);
      }
      var webSocketUrl = "ws:\/\/" + window.location.hostname + "/ws";
      var websocket;

      function initWebSocket() {
        websocket = new WebSocket(webSocketUrl);
        websocket.onopen = function (event) {};
        websocket.onclose = function (event) {
          setTimeout(initWebSocket, 2000);
        };
        websocket.onmessage = function (event) {};
      }

      function onTouchStartAndEnd(value) {
        websocket.send(value);
        console.log(value);
      }

      window.onload = initWebSocket;
      document
        .getElementById("mainTable")
        .addEventListener("touchend", function (event) {
          event.preventDefault();
        });
    </script>   
    </body>
</html>  
)HTMLHOMEPAGE";


void rotateMotor(int motorNumber, int motorDirection)
{
  if (motorDirection == FORWARD)
  {
    digitalWrite(motorPins[motorNumber].pinIN1, HIGH);
    digitalWrite(motorPins[motorNumber].pinIN2, LOW);    
  }
  else if (motorDirection == BACKWARD)
  {
    digitalWrite(motorPins[motorNumber].pinIN1, LOW);
    digitalWrite(motorPins[motorNumber].pinIN2, HIGH);     
  }
  else
  {
    digitalWrite(motorPins[motorNumber].pinIN1, HIGH);
    digitalWrite(motorPins[motorNumber].pinIN2, HIGH);       
  }
}

void processCarMovement(String inputValue)
{
  Serial.printf("Got value as %s %d\n", inputValue.c_str(), inputValue.toInt());  
  switch(inputValue.toInt())
  {

    case UP:
      rotateMotor(RIGHT_MOTOR, FORWARD);
      rotateMotor(LEFT_MOTOR, FORWARD);
      Serial.println("UP");                  
      break;
  
    case DOWN:
      rotateMotor(RIGHT_MOTOR, BACKWARD);
      rotateMotor(LEFT_MOTOR, BACKWARD);
      Serial.println("DOWN");   
      break;
  
    case TURN_LEFT:
      rotateMotor(LEFT_MOTOR, FORWARD);
      rotateMotor(RIGHT_MOTOR, BACKWARD);
      Serial.println("LEFT");  
      break;
  
    case TURN_RIGHT:
      rotateMotor(RIGHT_MOTOR, FORWARD);
      rotateMotor(LEFT_MOTOR, BACKWARD); 
      Serial.println("RIGHT");   
      break;

    case OPEN:
      relay1Active = true;
      relay2Active = false;
      digitalWrite(RELAY_PIN1, LOW);  // Turn ON Relay 1
      digitalWrite(RELAY_PIN2, HIGH); // Ensure Relay 2 is OFF   
      Serial.println("OPEN");  
      break;
    case CLOSE:
      relay1Active = false;
      relay2Active = true;
      digitalWrite(RELAY_PIN1, HIGH); // Ensure Relay 1 is OFF
      digitalWrite(RELAY_PIN2, LOW);  // Turn ON Relay 2
      Serial.println("CLOSE");   
      break;
  
    case STOP:
     rotateMotor(RIGHT_MOTOR, STOP);
      rotateMotor(LEFT_MOTOR, STOP); 
      Serial.println("STOP");     
      break;
  
    default:
      rotateMotor(RIGHT_MOTOR, STOP);
      rotateMotor(LEFT_MOTOR, STOP);     
      break;
  }
}

void handleRoot(AsyncWebServerRequest *request) 
{
  request->send_P(200, "text/html", htmlHomePage);
}

void handleNotFound(AsyncWebServerRequest *request) 
{
    request->send(404, "text/plain", "File Not Found");
}


void onWebSocketEvent(AsyncWebSocket *server, 
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
      //client->text(getRelayPinsStatusJson(ALL_RELAY_PINS_INDEX));
      break;
    case WS_EVT_DISCONNECT:
      Serial.printf("WebSocket client #%u disconnected\n", client->id());
      processCarMovement("0");
      break;
    case WS_EVT_DATA:
      AwsFrameInfo *info;
      info = (AwsFrameInfo*)arg;
      if (info->final && info->index == 0 && info->len == len && info->opcode == WS_TEXT) 
      {
        std::string myData = "";
        myData.assign((char *)data, len);
        processCarMovement(myData.c_str());       
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
  for (int i = 0; i < motorPins.size(); i++)
  {
    pinMode(motorPins[i].pinIN1, OUTPUT);
    pinMode(motorPins[i].pinIN2, OUTPUT);  
    rotateMotor(i, STOP);  
  }
}

void setup() {
  setUpPinModes();
  Serial.begin(115200);

  WiFi.softAP(ssid,password);
  IPAddress IP = WiFi.softAPIP();
  Serial.print("AP IP address: ");
  Serial.println(IP);

  server.on("/", HTTP_GET, handleRoot);
  server.onNotFound(handleNotFound);
  
  ws.onEvent(onWebSocketEvent);
  server.addHandler(&ws);
  
  server.begin();
  Serial.println("HTTP server started");

  // Set pin modes
  pinMode(RELAY_PIN1, OUTPUT);
  pinMode(RELAY_PIN2, OUTPUT);
  pinMode(CONTROL_SWITCH, INPUT_PULLUP);
  pinMode(LIMIT_SWITCH1, INPUT_PULLUP);
  pinMode(LIMIT_SWITCH2, INPUT_PULLUP);

  // Initialize relays to OFF (HIGH state since LOW turns them ON)
  digitalWrite(RELAY_PIN1, HIGH);
  digitalWrite(RELAY_PIN2, HIGH);
}

void loop() {
  ws.cleanupClients();
  static bool lastControlState = HIGH; // Stores the last state of the control switch

  // Read pin states
  bool currentControlState = digitalRead(CONTROL_SWITCH);
  bool limitSwitch1State = digitalRead(LIMIT_SWITCH1);
  bool limitSwitch2State = digitalRead(LIMIT_SWITCH2);

  // Detect if the control switch was pressed
  if (lastControlState == HIGH && currentControlState == LOW) {
    controlState = !controlState; // Toggle control state

    if (controlState) {
      relay1Active = true;
      relay2Active = false;
      digitalWrite(RELAY_PIN1, LOW);  // Turn ON Relay 1
      digitalWrite(RELAY_PIN2, HIGH); // Ensure Relay 2 is OFF
    } else {
      relay1Active = false;
      relay2Active = true;
      digitalWrite(RELAY_PIN1, HIGH); // Ensure Relay 1 is OFF
      digitalWrite(RELAY_PIN2, LOW);  // Turn ON Relay 2
    }
  }

  // Update the last control state
  lastControlState = currentControlState;

  // Handle Relay 1 logic
  if (relay1Active) {
    if (limitSwitch1State == LOW) { // Limit switch 1 is triggered
      digitalWrite(RELAY_PIN1, HIGH); // Turn OFF Relay 1
      relay1Active = false;
    }
  }

  // Handle Relay 2 logic
  if (relay2Active) {
    if (limitSwitch2State == LOW) { // Limit switch 2 is triggered
      digitalWrite(RELAY_PIN2, HIGH); // Turn OFF Relay 2
      relay2Active = false;
    }
  }
}
