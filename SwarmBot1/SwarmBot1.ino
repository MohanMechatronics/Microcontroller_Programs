#include <ESP8266WiFi.h>
#include <espnow.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define CHANNEL 0

const int motor1A = D1;
const int motor1B = D2;
const int motor2A = D7;
const int motor2B = D8;

#define I2C_SDA D6
#define I2C_SCL D5
TwoWire I2Cbus;

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels
// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
#define OLED_RESET     -1 // Reset pin # (or -1 if sharing Arduino reset pin)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &I2Cbus, OLED_RESET);

#include "eyes.h"
int xp = 16;
int mood = 1;

unsigned long previousMillis7 = 0, actionMillis7 = 0;
unsigned long previousMillis8 = 0, actionMillis8 = 0;
unsigned long previousMillis9 = 0, actionMillis9 = 0;
unsigned long previousMillis10 = 0, actionMillis10 = 0;

const long stopInterval = 10000;
const long forwardSS = 2000;

bool isMoving7 = false, isStopping7 = false;
bool isMoving8 = false, isStopping8 = false;
bool isMoving9 = false, isStopping9 = false;
bool isMoving10 = false, isStopping10 = false;

void OnDataRecv(uint8_t *mac_addr, uint8_t *data, uint8_t data_len); // Corrected function signature

void InitESPNow() {
  WiFi.disconnect();
  if (esp_now_init() == ERR_OK) {
    Serial.println("ESPNow Init Success");
  }
  else {
    Serial.println("ESPNow Init Failed");
    ESP.restart();
  }
}

void configDeviceAP() {
  const char *SSID = "Node_1";
  bool result = WiFi.softAP(SSID, "password", CHANNEL, 0);
  if (!result) {
    Serial.println("AP Config failed.");
  } else {
    Serial.println("AP Config Success. Broadcasting with AP: " + String(SSID));
  }
}

void setup() {
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

  pinMode(motor1A, OUTPUT);
  pinMode(motor1B, OUTPUT);
  pinMode(motor2A, OUTPUT);
  pinMode(motor2B, OUTPUT);

  Serial.println("ESPNow/Basic/Slave Example");
  WiFi.mode(WIFI_AP);
  configDeviceAP();
  Serial.print("AP MAC: "); Serial.println(WiFi.softAPmacAddress());
  InitESPNow();
  esp_now_register_recv_cb(OnDataRecv);
}

void OnDataRecv(uint8_t *mac_addr, uint8_t *data, uint8_t data_len) { // Corrected function signature
  char macStr[18];
  snprintf(macStr, sizeof(macStr), "%02x:%02x:%02x:%02x:%02x:%02x",
           mac_addr[0], mac_addr[1], mac_addr[2], mac_addr[3], mac_addr[4], mac_addr[5]);
  Serial.print("Last Packet Recv from: "); Serial.println(macStr);
  Serial.print("Last Packet Recv Data: "); Serial.println(*data);
  Serial.println("");

  unsigned long currentMillis = millis(); // Track the current time

  switch (*data) {
    case 1: //FORWARD
      FORWARD();                  
      break;
    case 2: //BACKWARD
      BACKWARD();                  
      break;
    case 3: //RIGHT 
      RIGHT();                
      break;
    case 4: //LEFT
      LEFT();              
      break;
    case 7:
    if (!isMoving7 && !isStopping7) {
        BACKWARD();                    
        isMoving7 = true;             
        actionMillis7 = currentMillis; 
      }

      if (isMoving7 && (currentMillis - actionMillis7 >= forwardSS)) {
        STOP();                       
        isMoving7 = false;           
        isStopping7 = true;           
        previousMillis7 = currentMillis; 
      }

      if (isStopping7 && (currentMillis - previousMillis7 >= stopInterval)) {
        isStopping7 = false;
      }
      break;

    case 8:
      if (!isMoving8 && !isStopping8) {
        FORWARD();                  
        isMoving8 = true;       
        actionMillis8 = currentMillis; 
      }

      if (isMoving8 && (currentMillis - actionMillis8 >= forwardSS)) {
        STOP();
        isMoving8 = false;    
        isStopping8 = true;        
        previousMillis8 = currentMillis; 
      }

      if (isStopping8 && (currentMillis - previousMillis8 >= stopInterval)) {
        isStopping8 = false;
      }
      break;
    case 9:
      if (!isMoving9 && !isStopping9) {
        LEFT();
        isMoving9 = true;
        actionMillis9 = currentMillis;
      }
      if (isMoving9 && (currentMillis - actionMillis9 >= 500)) { // Forward 1 second
        BACKWARD();
        actionMillis9 = currentMillis;
        isMoving9 = false;
        isStopping9 = true;
      }
      if (isStopping9 && (currentMillis - actionMillis9 >= 1000)) { // Left 0.5 second
        RIGHT();
        actionMillis9 = currentMillis;
        isMoving9 = true;
        isStopping9 = false;
      }
      if (isMoving9 && (currentMillis - actionMillis9 >= 500)) { // Forward 1 second
        BACKWARD();
        actionMillis9 = currentMillis;
        isMoving9 = false;
        isStopping9 = true;
      }
      if (isStopping9 && (currentMillis - actionMillis9 >= 1000)) { // Right 0.5 second
        STOP();
        isMoving9 = false;
        isStopping9 = false;
      }

      if (isStopping9 && (currentMillis - previousMillis9 >= stopInterval)) {
        isStopping9 = false;
      }
      break;

    case 10:
      if (!isMoving10 && !isStopping10) {
        FORWARD();
        isMoving10 = true;
        actionMillis10 = currentMillis;
      }
      if (isMoving10 && (currentMillis - actionMillis10 >= 1000)) { // Forward 1 second
        LEFT();
        actionMillis10 = currentMillis;
        isMoving10 = false;
        isStopping10 = true;
      }
      if (isStopping10 && (currentMillis - actionMillis10 >= 500)) { // Left 0.5 second
        FORWARD();
        actionMillis10 = currentMillis;
        isMoving10 = true;
        isStopping10 = false;
      }
      if (isMoving10 && (currentMillis - actionMillis10 >= 1000)) { // Forward 1 second
        RIGHT();
        actionMillis10 = currentMillis;
        isMoving10 = false;
        isStopping10 = true;
      }
      if (isStopping10 && (currentMillis - actionMillis10 >= 500)) { // Right 0.5 second
        STOP();
        isMoving10 = false;
        isStopping10 = false;
      }

      if (isStopping10 && (currentMillis - previousMillis10 >= stopInterval)) {
        isStopping10 = false;
      }
      break;
    case 0:
      STOP();                  
      break;
    default:
      STOP();                  
      break;
  }
}
void  FORWARD(){
  //FORWARD
  digitalWrite(motor1A, HIGH);
  digitalWrite(motor1B, LOW);
  digitalWrite(motor2A, HIGH);
  digitalWrite(motor2B, LOW);
}
void BACKWARD(){
  // BACKWARD
  digitalWrite(motor1A, LOW);
  digitalWrite(motor1B, HIGH);
  digitalWrite(motor2A, LOW);
  digitalWrite(motor2B, HIGH);
}
void LEFT(){
  // LEFT
  digitalWrite(motor1A, LOW);
  digitalWrite(motor1B, HIGH);
  digitalWrite(motor2A, HIGH);
  digitalWrite(motor2B, LOW);
}
void RIGHT(){
  // RIGHT
  digitalWrite(motor1A, HIGH);
  digitalWrite(motor1B, LOW);
  digitalWrite(motor2A, LOW);
  digitalWrite(motor2B, HIGH);
}
void STOP(){
  // STOP
  digitalWrite(motor1A, LOW);
  digitalWrite(motor1B, LOW);
  digitalWrite(motor2A, LOW);
  digitalWrite(motor2B, LOW);
}

void loop() {
  // Chill
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
