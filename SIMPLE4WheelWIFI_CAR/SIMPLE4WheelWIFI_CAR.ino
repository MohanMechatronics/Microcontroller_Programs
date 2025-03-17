#include <Arduino.h>
#include <WiFi.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>

#define UP 1
#define DOWN 2
#define TURN_LEFT 3
#define TURN_RIGHT 4
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
  {14, 27},  //LEFT_MOTOR
  {26, 25},  //RIGHT_MOTOR 
};

const char* ssid     = "GENERAL_BOT";
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
    <h2 style="color: rgb(255, 255, 255); text-align: center">GENERAL</h2>
    <h2 style="color: rgb(255, 255, 255); text-align: center">
      MECHATRONICS
    </h2>
    <div class="id">
      <table
        class="mainTable"
        style="width: 340px; margin: auto; table-layout: fixed"
        cellspacing="20"
      >
        <tr>
          <td>
            <span>
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
          <td>
            <span>
          </td>
        </tr>

        <tr>
          <td
            ontouchstart='onTouchStartAndEnd("3")'
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
            ontouchstart='onTouchStartAndEnd("4")'
            ontouchend='onTouchStartAndEnd("0")'
          >
            <span class="arrows rarrow" style="display: block">&#8680;</span>
          </td>
        </tr>

        <tr>
          <td>
            <span>
          </td>
          <td
            ontouchstart='onTouchStartAndEnd("2")'
            ontouchend='onTouchStartAndEnd("0")'
          >
            <span class="arrows downarrow" style="display: block">&#8681;</span>
          </td>
          <td>
            <span>
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
    digitalWrite(motorPins[motorNumber].pinIN1, LOW);
    digitalWrite(motorPins[motorNumber].pinIN2, LOW);       
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
      break;
  
    case DOWN:
      rotateMotor(RIGHT_MOTOR, BACKWARD);
      rotateMotor(LEFT_MOTOR, BACKWARD);   
      break;
  
    case TURN_LEFT:
      rotateMotor(LEFT_MOTOR, FORWARD);   
      break;
  
    case TURN_RIGHT:
      rotateMotor(RIGHT_MOTOR, FORWARD);    
      break;
  
    case STOP:
     rotateMotor(RIGHT_MOTOR, STOP);
      rotateMotor(LEFT_MOTOR, STOP);    
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
}

void loop() 
{
  ws.cleanupClients();
}