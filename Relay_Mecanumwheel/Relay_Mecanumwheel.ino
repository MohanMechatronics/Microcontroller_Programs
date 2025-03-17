#include <Arduino.h>
#ifdef ESP32
#include <WiFi.h>
#include <AsyncTCP.h>
#elif defined(ESP8266)
#include <ESP8266WiFi.h>
#include <ESPAsyncTCP.h>
#endif
#include <ESPAsyncWebServer.h>
#include <Wire.h>
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
int mood=1;

#define UP 1
#define DOWN 2
#define LEFT 3
#define RIGHT 4
#define UP_LEFT 5
#define UP_RIGHT 6
#define DOWN_LEFT 7
#define DOWN_RIGHT 8
#define TURN_LEFT 9
#define TURN_RIGHT 10
#define STOP 0

#define FRONT_RIGHT_MOTOR 0
#define BACK_RIGHT_MOTOR 1
#define FRONT_LEFT_MOTOR 2
#define BACK_LEFT_MOTOR 3

#define FORWARD 1
#define BACKWARD -1

struct MOTOR_PINS
{
  int pinIN1;
  int pinIN2;    
};

std::vector<MOTOR_PINS> motorPins = 
{
  {13, 12},  //FRONT_RIGHT_MOTOR
  {14, 27},  //BACK_RIGHT_MOTOR
  {26, 25},  //FRONT_LEFT_MOTOR
  {33, 32},  //BACK_LEFT_MOTOR   
};

const char* ssid     = "MECHATRONICS";
const char* password     = "mechatronics";

AsyncWebServer server(80);
AsyncWebSocket ws("/ws");


const char* htmlHomePage PROGMEM = R"HTMLHOMEPAGE(
<!DOCTYPE html>
<html>
  <head>
    <meta
      name="viewport"
      content="width=device-width, initial-scale=1, maximum-scale=1, user-scalable=no"
    />
    <style>
      body {
        background: rgb(82, 82, 82);
      }
      .marrow:active {
        background-color: red;
      }
      .uparrow:active,
      .tlarrow:active,
      .larrow:active,
      .trarrow:active,
      .laarrow:active,
      .blarrow:active,
      .brarrow:active,
      .rarrow:active,
      .downarrow:active,
      .rrarrow:active,
      .lrarrow:active {
        color: #000;
        transform: translatex(4px);
        background: linear-gradient(
          to top,
          rgb(255, 106, 131),
          rgb(77, 77, 255)
        );
        transition: 0.9s;
        box-shadow: 90px 90px 990px 990px #474747;
      }

      td {
        width: 50px;
        height: 50px;
        border-radius: 30px;
      }
      .maintable {
        font-size: 800px;
        z-index: 1;
      }
      .middle {
        background-color: rgb(119, 119, 119);
        width: 69px;
        height: 69px;
        border-radius: 90px;
      }
      .arrows {
        color: rgb(0, 191, 255);
        font-size: 60px;
        cursor: pointer;
        border-radius: 90px;
        z-index: 0;
      }

      .circularArrows {
        font-size: 60px;
        cursor: pointer;
      }
      .noselect {
        -webkit-touch-callout: none; /* iOS Safari */
        -webkit-user-select: none; /* Safari */
        -khtml-user-select: none; /* Konqueror HTML */
        -moz-user-select: none; /* Firefox */
        -ms-user-select: none; /* Internet Explorer/Edge */
        user-select: none; /* Non-prefixed version, currently
                                      supported by Chrome and Opera */
      }

      .lrarrow,
      .rrarrow {
        border-radius: 170px;
      }
    </style>
  </head>
  <body class="noselect" align="center">
    <h2 style="color: rgb(255, 255, 255); text-align: center">OTOMATIKS</h2>
    <h2 style="color: rgb(255, 255, 255); text-align: center">
      ROBOTICS ACADEMY FOR KIDS
    </h2>
    <div class="id">
      <table
        class="mainTable"
        style="width: 340px; margin: auto; table-layout: fixed"
        cellspacing="20"
      >
        <tr>
          <td
            id="box1"
            ontouchstart='onTouchStartAndEnd("6")'
            ontouchend='onTouchStartAndEnd("0")'
          >
            <span
              class="arrows tlarrow"
              onclick="downarrow()"
              style="display: block"
              >&#8598;</span
            >
          </td>
          <td
            ontouchstart='onTouchStartAndEnd("1")'
            ontouchend='onTouchStartAndEnd("0")'
          >
            <span
              class="arrows uparrow"
              onclick="downarrow()"
              style="display: block"
              >&#8679;</span
            >
          </td>
          <td
            ontouchstart='onTouchStartAndEnd("5")'
            ontouchend='onTouchStartAndEnd("0")'
          >
            <span
              class="arrows trarrow"
              onclick="downarrow()"
              style="display: block"
              >&#8599;</span
            >
          </td>
        </tr>

        <tr>
          <td
            ontouchstart='onTouchStartAndEnd("4")'
            ontouchend='onTouchStartAndEnd("0")'
          >
            <span
              class="arrows larrow"
              onclick="downarrow()"
              style="display: block"
              >&#8678;</span
            >
          </td>
          <td class="middle marrow" onclick="stop()">STOP</td>
          <td
            ontouchstart='onTouchStartAndEnd("3")'
            ontouchend='onTouchStartAndEnd("0")'
          >
            <span class="arrows rarrow" style="display: block">&#8680;</span>
          </td>
        </tr>

        <tr>
          <td
            ontouchstart='onTouchStartAndEnd("8")'
            ontouchend='onTouchStartAndEnd("0")'
          >
            <span class="arrows blarrow" style="display: block">&#8601;</span>
          </td>
          <td
            ontouchstart='onTouchStartAndEnd("2")'
            ontouchend='onTouchStartAndEnd("0")'
          >
            <span class="arrows downarrow" style="display: block">&#8681;</span>
          </td>
          <td
            ontouchstart='onTouchStartAndEnd("7")'
            ontouchend='onTouchStartAndEnd("0")'
          >
            <span class="arrows brarrow" style="display: block">&#8600;</span>
          </td>
        </tr>

        <tr>
          <td
            ontouchstart='onTouchStartAndEnd("10")'
            ontouchend='onTouchStartAndEnd("0")'
          >
            <span class="circularArrows rrarrow" style="display: block"
              >&#8634;</span
            >
          </td>
          <td style="display: inline-table"></td>
          <td
            ontouchstart='onTouchStartAndEnd("9")'
            ontouchend='onTouchStartAndEnd("0")'
          >
            <span class="circularArrows lrarrow" style="display: block">
              &#8635;
            </span>
          </td>
        </tr>
      </table>
    </div>
    <script>
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
      rotateMotor(FRONT_RIGHT_MOTOR, FORWARD);
      rotateMotor(BACK_RIGHT_MOTOR, FORWARD);
      rotateMotor(FRONT_LEFT_MOTOR, FORWARD);
      rotateMotor(BACK_LEFT_MOTOR, FORWARD);                  
      break;
  
    case DOWN:
      rotateMotor(FRONT_RIGHT_MOTOR, BACKWARD);
      rotateMotor(BACK_RIGHT_MOTOR, BACKWARD);
      rotateMotor(FRONT_LEFT_MOTOR, BACKWARD);
      rotateMotor(BACK_LEFT_MOTOR, BACKWARD);   
      break;
  
    case LEFT:
      rotateMotor(FRONT_RIGHT_MOTOR, FORWARD);
      rotateMotor(BACK_RIGHT_MOTOR, BACKWARD);
      rotateMotor(FRONT_LEFT_MOTOR, BACKWARD);
      rotateMotor(BACK_LEFT_MOTOR, FORWARD);   
      break;
  
    case RIGHT:
      rotateMotor(FRONT_RIGHT_MOTOR, BACKWARD);
      rotateMotor(BACK_RIGHT_MOTOR, FORWARD);
      rotateMotor(FRONT_LEFT_MOTOR, FORWARD);
      rotateMotor(BACK_LEFT_MOTOR, BACKWARD);  
      break;
  
    case UP_LEFT:
      rotateMotor(FRONT_RIGHT_MOTOR, FORWARD);
      rotateMotor(BACK_RIGHT_MOTOR, STOP);
      rotateMotor(FRONT_LEFT_MOTOR, STOP);
      rotateMotor(BACK_LEFT_MOTOR, FORWARD);  
      break;
  
    case UP_RIGHT:
      rotateMotor(FRONT_RIGHT_MOTOR, STOP);
      rotateMotor(BACK_RIGHT_MOTOR, FORWARD);
      rotateMotor(FRONT_LEFT_MOTOR, FORWARD);
      rotateMotor(BACK_LEFT_MOTOR, STOP);  
      break;
  
    case DOWN_LEFT:
      rotateMotor(FRONT_RIGHT_MOTOR, STOP);
      rotateMotor(BACK_RIGHT_MOTOR, BACKWARD);
      rotateMotor(FRONT_LEFT_MOTOR, BACKWARD);
      rotateMotor(BACK_LEFT_MOTOR, STOP);   
      break;
  
    case DOWN_RIGHT:
      rotateMotor(FRONT_RIGHT_MOTOR, BACKWARD);
      rotateMotor(BACK_RIGHT_MOTOR, STOP);
      rotateMotor(FRONT_LEFT_MOTOR, STOP);
      rotateMotor(BACK_LEFT_MOTOR, BACKWARD);   
      break;
  
    case TURN_LEFT:
      rotateMotor(FRONT_RIGHT_MOTOR, FORWARD);
      rotateMotor(BACK_RIGHT_MOTOR, FORWARD);
      rotateMotor(FRONT_LEFT_MOTOR, BACKWARD);
      rotateMotor(BACK_LEFT_MOTOR, BACKWARD);  
      break;
  
    case TURN_RIGHT:
      rotateMotor(FRONT_RIGHT_MOTOR, BACKWARD);
      rotateMotor(BACK_RIGHT_MOTOR, BACKWARD);
      rotateMotor(FRONT_LEFT_MOTOR, FORWARD);
      rotateMotor(BACK_LEFT_MOTOR, FORWARD);   
      break;
  
    case STOP:
      rotateMotor(FRONT_RIGHT_MOTOR, STOP);
      rotateMotor(BACK_RIGHT_MOTOR, STOP);
      rotateMotor(FRONT_LEFT_MOTOR, STOP);
      rotateMotor(BACK_LEFT_MOTOR, STOP);    
      break;
  
    default:
      rotateMotor(FRONT_RIGHT_MOTOR, STOP);
      rotateMotor(BACK_RIGHT_MOTOR, STOP);
      rotateMotor(FRONT_LEFT_MOTOR, STOP);
      rotateMotor(BACK_LEFT_MOTOR, STOP);    
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
  
  ws.onEvent(onWebSocketEvent);
  server.addHandler(&ws);
  
  server.begin();
  Serial.println("HTTP server started");
  I2Cbus.begin(I2C_SDA, I2C_SCL, 100000);
  Serial.println("Booting");

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
}

void loop() 
{
  ws.cleanupClients();
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
