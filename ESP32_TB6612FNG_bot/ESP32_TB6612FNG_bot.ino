#include <Arduino.h>
#ifdef ESP32
#include <WiFi.h>
#include <AsyncTCP.h>
#elif defined(ESP8266)
#include <ESP8266WiFi.h>
#include <ESPAsyncTCP.h>
#endif
#include <ESPAsyncWebServer.h>
#include <iostream>
#include <sstream>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define I2C_SDA 21
#define I2C_SCL 22
TwoWire I2Cbus = TwoWire(0);

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels
// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
#define OLED_RESET     -1 // Reset pin # (or -1 if sharing Arduino reset pin)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &I2Cbus, OLED_RESET);

#include "eyes.h"
int xp=16;
int mood=3;

#define STBY 27

struct MOTOR_PINS
{
  int pinEn;  
  int pinIN1;
  int pinIN2;    
};

std::vector<MOTOR_PINS> motorPins = 
{
  {16, 12, 14},  //RIGHT_MOTOR Pins (EnA, IN1, IN2)
  {26, 25, 4},  //LEFT_MOTOR  Pins (EnB, IN3, IN4)
};

#define UP 1
#define DOWN 2
#define LEFT 3
#define RIGHT 4
#define STOP 0

#define RIGHT_MOTOR 0
#define LEFT_MOTOR 1

#define FORWARD 1
#define BACKWARD -1

const int PWMFreq = 1000; /* 1 KHz */
const int PWMResolution = 8;
const int PWMSpeedChannel = 4;

const char* ssid     = "PRO_BOT";
const char* password = "gunasree";

AsyncWebServer server(80);
AsyncWebSocket wsCarInput("/CarInput");

const char* htmlHomePage PROGMEM = R"HTMLHOMEPAGE(
<!DOCTYPE html>
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
.slidecontainer {
      width: 100%;
    }

    .slider {
      -webkit-appearance: none;
      width: 50%;
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
        /* ----------------------------------- */
    </style>
    </head>
    <body style="background-color:#000000;" class="noselect" align="center">
    <h2 style="color: rgb(255, 255, 255); text-align: center">PRO-BOT</h2>
    <h2 style="color: rgb(255, 255, 255); text-align: center">
      MECHATRONICS
    </h2>
    <br/>
    <div align=center> 
    <button class=" btn btn4 triangle-up" id="forward" ontouchstart='sendButtonInput("MoveCar","1")' ontouchend='sendButtonInput("MoveCar","0")'></button>
    </div>
    <br/>

    <div align=center> 
    <button class="btn btn2 triangle-left" id="turnleft" ontouchstart='sendButtonInput("MoveCar","3")' ontouchend='sendButtonInput("MoveCar","0")'></button>
    <button class="btn btn5 stop" id="stop"></button>
    <button class="btn btn1 triangle-right" id="turnright" ontouchstart='sendButtonInput("MoveCar","4")' ontouchend='sendButtonInput("MoveCar","0")'></button>
    </div>
    <br/>

    <div align=center> 
    <button class="btn btn3 triangle-down" id="backward" ontouchstart='sendButtonInput("MoveCar","2")' ontouchend='sendButtonInput("MoveCar","0")'></button>
    </div>
    
    <div align=center class="slidecontainer">
<h3 style="color: #FFFFFF;font-size:25px"><b>Speed-Control</b></h3>
      <input type="range" min="0" max="255" value="150" class="slider" id="Speed" oninput='sendButtonInput("Speed",value)'>
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
      var webSocketCarInputUrl = "ws:\/\/" + window.location.hostname + "/CarInput";      
      var websocketCarInput;
      
      function initCarInputWebSocket() 
      {
        websocketCarInput = new WebSocket(webSocketCarInputUrl);
        websocketCarInput.onopen    = function(event)
        {
          var speedButton = document.getElementById("Speed");
          sendButtonInput("Speed", speedButton.value);
        };
        websocketCarInput.onclose   = function(event){setTimeout(initCarInputWebSocket, 2000);};
        websocketCarInput.onmessage = function(event){};        
      }
      
      function sendButtonInput(key, value) 
      {
        var data = key + "," + value;
        websocketCarInput.send(data);
      }
    
      window.onload = initCarInputWebSocket;
      document.getElementById("mainTable").addEventListener("touchend", function(event){
        event.preventDefault()
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
    digitalWrite(motorPins[motorNumber].pinIN1, LOW);
    digitalWrite(motorPins[motorNumber].pinIN2, LOW);       
  }
}

void moveCar(int inputValue)
{
  Serial.printf("Got value as %d\n", inputValue);  
  switch(inputValue)
  {

    case UP:
      rotateMotor(RIGHT_MOTOR, FORWARD);
      rotateMotor(LEFT_MOTOR, FORWARD);
      mood = 3;                  
      break;
  
    case DOWN:
      rotateMotor(RIGHT_MOTOR, BACKWARD);
      rotateMotor(LEFT_MOTOR, BACKWARD);
      mood = 5;  
      break;
  
    case LEFT:
      rotateMotor(RIGHT_MOTOR, FORWARD);
      rotateMotor(LEFT_MOTOR, BACKWARD);
      mood = 4;
      break;
  
    case RIGHT:
      rotateMotor(RIGHT_MOTOR, BACKWARD);
      rotateMotor(LEFT_MOTOR, FORWARD);
      mood = 2; 
      break;
 
    case STOP:
      rotateMotor(RIGHT_MOTOR, STOP);
      rotateMotor(LEFT_MOTOR, STOP);
      mood = 1;    
      break;
  
    default:
      rotateMotor(RIGHT_MOTOR, STOP);
      rotateMotor(LEFT_MOTOR, STOP);
      mood = 1;    
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

void onCarInputWebSocketEvent(AsyncWebSocket *server, 
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
      moveCar(STOP);
      break;
    case WS_EVT_DATA:
      AwsFrameInfo *info;
      info = (AwsFrameInfo*)arg;
      if (info->final && info->index == 0 && info->len == len && info->opcode == WS_TEXT) 
      {
        std::string myData = "";
        myData.assign((char *)data, len);
        std::istringstream ss(myData);
        std::string key, value;
        std::getline(ss, key, ',');
        std::getline(ss, value, ',');
        Serial.printf("Key [%s] Value[%s]\n", key.c_str(), value.c_str()); 
        int valueInt = atoi(value.c_str());     
        if (key == "MoveCar")
        {
          moveCar(valueInt);        
        }
        else if (key == "Speed")
        {
          ledcWrite(PWMSpeedChannel, valueInt);
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
  //Set up PWM
  ledcSetup(PWMSpeedChannel, PWMFreq, PWMResolution);
  pinMode(STBY, OUTPUT);
  digitalWrite(STBY, HIGH);
  for (int i = 0; i < motorPins.size(); i++)
  {
    pinMode(motorPins[i].pinEn, OUTPUT);    
    pinMode(motorPins[i].pinIN1, OUTPUT);
    pinMode(motorPins[i].pinIN2, OUTPUT);  

    /* Attach the PWM Channel to the motor enb Pin */
    ledcAttachPin(motorPins[i].pinEn, PWMSpeedChannel);
  }
  moveCar(STOP);
}


void setup(void) 
{
  setUpPinModes();
  Serial.begin(115200);

  // Initialize I2C with our defined pins
  I2Cbus.begin(I2C_SDA, I2C_SCL, 100000);

  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3c)) { // Address 0x3D for 128x64
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); // Don't proceed, loop forever
  }

  // Show initial display buffer contents on the screen --
  // the library initializes this with an Adafruit splash screen.
  display.display();
  
  // Clear the buffer
  display.clearDisplay();
  display.display();

  WiFi.softAP(ssid, password);
  IPAddress IP = WiFi.softAPIP();
  Serial.print("AP IP address: ");
  Serial.println(IP);

  server.on("/", HTTP_GET, handleRoot);
  server.onNotFound(handleNotFound);
      
  wsCarInput.onEvent(onCarInputWebSocketEvent);
  server.addHandler(&wsCarInput);

  server.begin();
  Serial.println("HTTP server started");

}

void loop() 
{
  wsCarInput.cleanupClients();
  int n;
  static int xd=0;
  static int espera=0;
  static int step=0;
  int x1,x2;
  if (espera>0) {
    espera--;
    delay(1);
  } else {
    x1=   xd+ (xp>16? (16+2*(xp-16)):xp);
    x2=64+xd+ (xp<16? (-16+(xp*2))  :xp);
    switch (step){
      case 0:
       display.clearDisplay(); // Clear the display buffer
       if (xp<6) { 
               display.drawBitmap(x1, 8, peyes[mood][2][0], 32, 32, WHITE);
               display.drawBitmap(x2, 8, peyes[mood][1][1], 32, 32, WHITE);
       } else if (xp<26) {
               display.drawBitmap(x1, 8, peyes[mood][0][0], 32, 32, WHITE);
               display.drawBitmap(x2, 8, peyes[mood][0][1], 32, 32, WHITE); 
       } else {
               display.drawBitmap(x1, 8, peyes[mood][1][0], 32, 32, WHITE); 
               display.drawBitmap(x2, 8, peyes[mood][2][1], 32, 32, WHITE);
       }
       display.display();
       espera=random(250, 1000);
       n=random(0,7);
       if (n==6) {
          step=1;
       } else {
          step=2;
       }
       break;
      case 1:
       display.clearDisplay(); // Clear the display buffer
       display.drawBitmap(x1, 8, eye0, 32, 32, WHITE);
       display.drawBitmap(x2, 8, eye0, 32, 32, WHITE);
       display.display();
       espera=100;
       step=0;
       break;
      case 2:
       n=random(0,10);
       if (n<5) xd--;
       if (n>5) xd++;
       if (xd<-4) xd=-3;
       if (xd>4) xd=3; 
       espera=0;
       step=0;
       break;
    }
  }
}
