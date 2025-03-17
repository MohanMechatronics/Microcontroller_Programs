#include <esp_now.h>
#include <WiFi.h>
#include <Arduino.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <ArduinoJson.h>
 

const char* ssid     = "Mini_Bot";
const char* password     = "mechatronics";

AsyncWebServer server(80);
AsyncWebSocket ws("/ws");

int ButtonValue;
int NodeState;
int BtnState;

// Global copies of slaves
esp_now_peer_info_t slave1;
esp_now_peer_info_t slave2;
esp_now_peer_info_t slave3;
esp_now_peer_info_t slave4;
esp_now_peer_info_t slave5;

#define CHANNEL 0
#define PRINTSCANRESULTS 0
#define DELETEBEFOREPAIR 0

// MAC addresses of the slaves

int mac1[6] = {0xBE, 0xDD, 0xC2, 0x57, 0x8B, 0x03}; // BE:DD:C2:57:8B:03 ESP8266
int mac2[6] = {0x08, 0xD1, 0xF9, 0x29, 0xC1, 0xFD}; // 08:D1:F9:29:C1:FD done ORANGE
// int mac3[6] = {0xD4, 0x8A, 0xFC, 0x9E, 0x83, 0xE1}; // D4:8A:FC:9E:83:E1 ALONE
int mac3[6] = {0x08, 0xD1, 0xF9, 0x37, 0x5C, 0x29}; // 08:D1:F9:37:5C:29 done BLUE
int mac4[6] = {0x08, 0xD1, 0xF9, 0xCC, 0x21, 0x7D}; // 08:D1:F9:CC:21:7D done GREY
int mac5[6] = {0xE8, 0x6B, 0xEA, 0xDE, 0xB3, 0xA5}; // E8:6B:EA:DE:B3:A5 done GREEN

// Init ESP Now with fallback
void InitESPNow() {
  WiFi.disconnect();
  if (esp_now_init() == ESP_OK) {
    Serial.println("ESPNow Init Success");
  }
  else {
    Serial.println("ESPNow Init Failed");
    ESP.restart(); // Restart ESP if initialization fails
  }
}

// Scan for slaves in AP mode
void ScanForSlaves() {
  memset(&slave1, 0, sizeof(slave1));
  memset(&slave2, 0, sizeof(slave2));
  memset(&slave3, 0, sizeof(slave3));
  memset(&slave4, 0, sizeof(slave4));
  memset(&slave5, 0, sizeof(slave5));

  // Set MAC addresses and other parameters for each slave
  for (int ii = 0; ii < 6; ++ii) {
    slave1.peer_addr[ii] = (uint8_t) mac1[ii];
    slave2.peer_addr[ii] = (uint8_t) mac2[ii];
    slave3.peer_addr[ii] = (uint8_t) mac3[ii];
    slave4.peer_addr[ii] = (uint8_t) mac4[ii];
    slave5.peer_addr[ii] = (uint8_t) mac5[ii];
  }

  // Set channel and encryption for each slave
  slave1.channel = CHANNEL;
  slave1.encrypt = 0;

  slave2.channel = CHANNEL;
  slave2.encrypt = 0;

  slave3.channel = CHANNEL;
  slave3.encrypt = 0;

  slave4.channel = CHANNEL;
  slave4.encrypt = 0;

  slave5.channel = CHANNEL;
  slave5.encrypt = 0;
  
  // Serial.println("Slaves Found, processing..");
}

// Check if the slave is already paired with the master.
// If not, pair the slave with master
bool manageSlave(esp_now_peer_info_t &slave) {
  if (slave.channel == CHANNEL) {
    if (DELETEBEFOREPAIR) {
      deletePeer(slave);
    }

    // Serial.print("Slave Status: ");
    bool exists = esp_now_is_peer_exist(slave.peer_addr);
    if (exists) {
      // Serial.println("Already Paired");
      return true;
    } else {
      esp_err_t addStatus = esp_now_add_peer(&slave);
      if (addStatus == ESP_OK) {
        Serial.println("Pair success");
        return true;
      } else {
        Serial.println("Pair failed");
        return false;
      }
    }
  } else {
    Serial.println("No Slave found to process");
    return false;
  }
}

void deletePeer(esp_now_peer_info_t &slave) {
  esp_err_t delStatus = esp_now_del_peer(slave.peer_addr);
  Serial.print("Slave Delete Status: ");
  if (delStatus == ESP_OK) {
    Serial.println("Success");
  } else {
    Serial.println("Failed");
  }
}

uint8_t data = 0;
// send data
void sendData(esp_now_peer_info_t &slave) {
  data = ButtonValue;

  const uint8_t *peer_addr = slave.peer_addr;
  Serial.print("Sending to Slave: "); Serial.println(peer_addr[5], HEX);
  Serial.print("Data: "); Serial.println(data);
  esp_err_t result = esp_now_send(peer_addr, (uint8_t *)&data, sizeof(data));

  Serial.print("Send Status: ");
  if (result == ESP_OK) {
    Serial.println("Success");
  } else {
    Serial.println("Failed");
  }
}

// callback when data is sent from Master to Slave
void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
  char macStr[18];
  snprintf(macStr, sizeof(macStr), "%02x:%02x:%02x:%02x:%02x:%02x",
           mac_addr[0], mac_addr[1], mac_addr[2], mac_addr[3], mac_addr[4], mac_addr[5]);
  // Serial.print("Last Packet Sent to: "); Serial.println(macStr);
  // Serial.print("Last Packet Send Status: "); Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Delivery Success" : "Delivery Fail");         
}

// send data to all slaves
void sendDataToSlaves() {

 // Check if the condition for sending data is met for each slave
  if (NodeState == 1 && BtnState == 1) {
    digitalWrite(13,HIGH);
  } else if (NodeState == 1 && BtnState == 0 ) {
    digitalWrite(13,LOW);
  }
  else if (NodeState == 2 && BtnState == 1 ) {
    digitalWrite(12,HIGH);
  } else if (NodeState == 2 && BtnState == 0 ) {
    digitalWrite(12,LOW);
  }
  else if (NodeState == 3 && BtnState == 1 ) {
    digitalWrite(14,HIGH);
  } else if (NodeState == 3 && BtnState == 0 ) {
    digitalWrite(14,LOW);
  }
  else if (NodeState == 4 && BtnState == 1 ) {
    digitalWrite(27,HIGH);
  } else if (NodeState == 4 && BtnState == 0 ) {
    digitalWrite(27,LOW);
  }
  else if (NodeState == 5 && BtnState == 1 ) {
    digitalWrite(26,HIGH);
  } else if (NodeState == 5 && BtnState == 0 ) {
    digitalWrite(26,LOW);
  }
  else if (NodeState == 11 && BtnState == 1 ) {
    digitalWrite(25,HIGH);
  } else if (NodeState == 11 && BtnState == 0 ) {
    digitalWrite(25,LOW);
  }
}


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

input:checked + .slider2::before {
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
.slaves{
    display: grid;
    width: 330px;
    gap: 30px;
    align-items: center;
    justify-content:space-between;
    grid-template-columns: 1fr 1fr 1fr ;
    position: absolute;
    bottom: 10%;
    left: 10%;
}
/* .all{
    background-color: var(--clr-neon);
} */
 .all::before {
    -webkit-transform: translateX(26px);
    -ms-transform: translateX(26px);
    transform: translateX(26px);
    background-color: rgb(89, 136, 237);

}
.nodeName{
  color: #ffff;
  position: relative;
  top:-30px;
}
        /* ----------------------------------- */
</style>
    </head>
    <body style="background-color:black;" class="noselect" align="center">
    <h2 style="color: rgb(255, 255, 255); text-align: center">T2v2</h2>
    <h2 style="color: rgb(255, 255, 255); text-align: center">
      MECHATRONICS
    </h2>
    <div align=center> 
    <button class=" btn btn4 triangle-up" id="forward" ontouchstart='onTouchStartAndEnd("1")'
            ontouchend='onTouchStartAndEnd("0")'></button>
    </div>
    <br/>

    <div align=center> 
    <button class="btn btn2 triangle-left" id="turnleft" ontouchstart='onTouchStartAndEnd("4")'
            ontouchend='onTouchStartAndEnd("0")'></button>
    <button class="btn btn5 stop" id="stop"></button>
    <button class="btn btn1 triangle-right" id="turnright" ontouchstart='onTouchStartAndEnd("3")'
            ontouchend='onTouchStartAndEnd("0")'></button>
    </div>
    <br/>

    <div align=center> 
    <button class="btn btn3 triangle-down" id="backward" ontouchstart='onTouchStartAndEnd("2")'
            ontouchend='onTouchStartAndEnd("0")'></button>
    </div>
    <br/><br/><br/><br/><br/><br/><br/>
    <div  class="slaves" align=center>
        <label class="switch">
          <sup class="nodeName">NODE 1</sup>
        <input type="checkbox" class="dis" id="NODE1" onchange="send_State_Cmd('1','NODE1')">
        <span class="slider2 round "></span>
      </label>

          <label class="switch">
            <sup class="nodeName">NODE 2</sup>
            <input type="checkbox" class="dis" id="NODE2" onchange="send_State_Cmd('2','NODE2')">
            <span class="slider2 round "></span>
          </label>
          <label class="switch">
            <sup class="nodeName">NODE 3</sup>
            <input type="checkbox" class="dis" id="NODE3" onchange="send_State_Cmd('3','NODE3')">
            <span class="slider2 round "></span>
          </label>
          <label class="switch">
          <sup class="nodeName">NODE 4</sup>
            <input type="checkbox" class="dis" id="NODE4" onchange="send_State_Cmd('4','NODE4')">
            <span class="slider2 round "></span>
          </label>
          <label class="switch">
          <sup class="nodeName">NODE 5</sup>

            <input type="checkbox" class="dis" id="NODE5" onchange="send_State_Cmd('5','NODE5')">
            <span class="slider2 round "></span>
          </label>
          <label class="switch">
          <sup class="nodeName">NODE 6</sup>

            <input type="checkbox" class="dis" id="NODE6" onchange="send_State_Cmd('6','NODE6')">
            <span class="slider2 round "></span>
          </label>
          <label class="switch">
          <sup class="nodeName">NODE 7</sup>

            <input type="checkbox" class="dis" id="NODE7" onchange="send_State_Cmd('7','NODE7')">
            <span class="slider2 round "></span>
          </label>
          <label class="switch">
          <sup class="nodeName">NODE 8</sup>

            <input type="checkbox" class="dis" id="NODE8" onchange="send_State_Cmd('8','NODE8')">
            <span class="slider2 round "></span>
          </label>
          <label class="switch">
          <sup class="nodeName">NODE 9</sup>

            <input type="checkbox" class="dis" id="NODE9" onchange="send_State_Cmd('9','NODE9')">
            <span class="slider2 round "></span>
          </label>
          <label class="switch">
          <sup class="nodeName">NODE 10</sup>

            <input type="checkbox" class="dis" id="NODE10" onchange="send_State_Cmd('10','NODE10')">
            <span class="slider2 round "></span>
          </label>

          <label>
            </label>
          <label class="switch" for="NODE11">
            <sup class="nodeName">ALL NODES</sup>

            <input type="checkbox" id="NODE11" onchange="send_State_Cmd('11','NODE11')">
            <span class="slider2 round "></span>
          </label>
    </div>
</br>



    <script>
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

      function send_State_Cmd(board, id) {
          var checkBox = document.getElementById(id);
          var btnState;
          if(id==='NODE11' && checkBox){
           btnState = checkBox.checked ? 1 : 0;
           //to change all the toggle buttons to checked 
           if(btnState) document.querySelectorAll('.dis').forEach((e)=>e.disabled=true)
           else document.querySelectorAll('.dis').forEach((e)=>e.disabled=false)
             document.querySelectorAll('.slider2').forEach((e)=>e.classList.toggle("all"))
          
            let allval=document.querySelectorAll('.switch')
            allval.forEach((e)=>{console.log('Board :ESP32Slave${e.childNodes[1].id.slice(4)} Button State :${btnState}');})
            
          }
        if (checkBox) {
          btnState = checkBox.checked ? 1 : 0;
          console.log("Board: " + board + ", Button State: " + btnState);
          
          var full_data = '{"Board" :'+board+',"ButtonState":'+btnState+'}';
          websocket.send(full_data);

          
    
        } else {
          console.error("Element with ID '" + id + "' not found.");
          websocket.send(full_data);
        }
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

void processMovement(String inputValue)
{
  Serial.printf("Got value as %s %d\n", inputValue.c_str(), inputValue.toInt()); 
  ButtonValue = inputValue.toInt();   
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
      processMovement("0");
      break;
    case WS_EVT_DATA:
      AwsFrameInfo *info;
      info = (AwsFrameInfo*)arg;
      if (info->final && info->index == 0 && info->len == len && info->opcode == WS_TEXT) 
      {
        std::string myData = "";
        myData.assign((char *)data, len);

       DynamicJsonDocument doc(200);
        // deserialize the data
       DeserializationError error = deserializeJson(doc, myData);
       // parse the parameters we expect to receive (TO-DO: error handling)
        // Test if parsing succeeds.
       if (error) {
         Serial.print("deserializeJson() failed: ");
         Serial.println(error.c_str());
          return;
        }

        // var full_data = '{"Board" :'+board+',"ButtonState":'+btnState+'}';
        BtnState = doc["ButtonState"];
        NodeState = doc["Board"];
        processMovement(myData.c_str());       
      }
      break;
    case WS_EVT_PONG:
    case WS_EVT_ERROR:
      break;
    default:
      break;  
  }
}

void setup() {
  Serial.begin(115200);
  WiFi.mode(WIFI_STA);
  Serial.print("STA MAC: "); Serial.println(WiFi.macAddress());
  InitESPNow();
  esp_now_register_send_cb(OnDataSent);

  pinMode(13,OUTPUT);
  pinMode(12,OUTPUT);
  pinMode(14,OUTPUT);
  pinMode(27,OUTPUT);
  pinMode(26,OUTPUT);
  pinMode(25,OUTPUT);
  
  WiFi.softAP(ssid,password);
  IPAddress IP = WiFi.softAPIP(

    
  );
  Serial.print("AP IP address: ");
  Serial.println(IP);

  server.on("/", HTTP_GET, handleRoot);
  server.onNotFound(handleNotFound);
  
  ws.onEvent(onWebSocketEvent);
  server.addHandler(&ws);
  
  server.begin();
  Serial.println("HTTP server started");
}

void loop() {
  delay(100); 
  ws.cleanupClients();
  ScanForSlaves();
  manageSlave(slave1);
  manageSlave(slave2);
  manageSlave(slave3);
  manageSlave(slave4);
  manageSlave(slave5);

  sendDataToSlaves();

  if(digitalRead(13)==HIGH){
    sendData(slave1);
  }if(digitalRead(12)==HIGH){
    sendData(slave2);
  }if(digitalRead(14)==HIGH){
    sendData(slave3);
  }if(digitalRead(27)==HIGH){
    sendData(slave4);
  }if(digitalRead(26)==HIGH){
    sendData(slave5);
  }if(digitalRead(25)==HIGH){
    sendData(slave1);
    sendData(slave2);
    sendData(slave3);
    sendData(slave4);
    sendData(slave5);
  }

}
