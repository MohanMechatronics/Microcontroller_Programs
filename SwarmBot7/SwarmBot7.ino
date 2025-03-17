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
int mood = 3;

unsigned long previousMillis5 = 0;       // Timer for stopping period for case 5 (10 seconds)
unsigned long actionMillis5 = 0;         // Timer for action in case 5 (2 seconds)
unsigned long previousMillis6 = 0;       // Timer for stopping period for case 6 (10 seconds)
unsigned long actionMillis6 = 0;         // Timer for action in case 6 (2 seconds)

unsigned long previousMillis7 = 0;       
unsigned long actionMillis7 = 0;        
unsigned long previousMillis8 = 0;       
unsigned long actionMillis8 = 0;

unsigned long previousMillis11 = 0;       
unsigned long actionMillis11 = 0;        
unsigned long previousMillis12 = 0;       
unsigned long actionMillis12 = 0;  

const long stopInterval = 10000;         // 10 seconds stop time
const long forwardDuration = 1000;          // 2 seconds for forward action
const long forwardSS = 2000;

bool isMoving5 = false;                  // Flag to track if robot is moving for case 5
bool isStopping5 = false;                // Flag to track stop state for case 5
bool isMoving6 = false;                  // Flag to track if robot is moving for case 6
bool isStopping6 = false;

bool isMoving7 = false;                  
bool isStopping7 = false;
bool isMoving8 = false;                  
bool isStopping8 = false;

bool isMoving11 = false;                  
bool isStopping11 = false;
bool isMoving12 = false;                  
bool isStopping12 = false;
  

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
  const char *SSID = "Node_7";
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
    case 3: //LEFT
      LEFT();              
      break;
    case 4: //RIGHT 
      RIGHT();                
      break;
    case 5:
    if (!isMoving5 && !isStopping5) {
        FORWARD();                    // Start moving forward for 2 seconds
        isMoving5 = true;             // Set flag to indicate the robot is moving
        actionMillis5 = currentMillis; // Record the time when action started
      }

      // Check if the robot has been moving for 2 seconds
      if (isMoving5 && (currentMillis - actionMillis5 >= forwardDuration)) {
        STOP();                       // Stop after 2 seconds of movement
        isMoving5 = false;            // Reset the moving flag
        isStopping5 = true;           // Set flag to indicate stop period starts
        previousMillis5 = currentMillis; // Record the time when stop period starts
      }

      // Check if the stop period of 10 seconds has passed
      if (isStopping5 && (currentMillis - previousMillis5 >= stopInterval)) {
        isStopping5 = false;          // Reset the stop flag after 10 seconds
      }
      break;

    case 6: // Case 6: Left for 2 seconds, stop for 100 seconds
      if (!isMoving6 && !isStopping6) {
        BACKWARD();                       // Start moving BACKWARD() for 2 seconds
        isMoving6 = true;             // Set flag to indicate the robot is moving
        actionMillis6 = currentMillis; // Record the time when action started
      }

      // Check if the robot has been moving for 2 seconds
      if (isMoving6 && (currentMillis - actionMillis6 >= forwardDuration)) {
        STOP();                       // Stop after 2 seconds of movement
        isMoving6 = false;            // Reset the moving flag
        isStopping6 = true;           // Set flag to indicate stop period starts
        previousMillis6 = currentMillis; // Record the time when stop period starts
      }

      // Check if the stop period of 10 seconds has passed
      if (isStopping6 && (currentMillis - previousMillis6 >= stopInterval)) {
        isStopping6 = false;          // Reset the stop flag after 10 seconds
      }
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
    case 11:
    if (!isMoving11 && !isStopping11) {
        FORWARD();                    
        isMoving11 = true;             
        actionMillis11 = currentMillis; 
      }

      if (isMoving11 && (currentMillis - actionMillis11 >= forwardDuration)) {
        STOP();                       
        isMoving11 = false;           
        isStopping11 = true;           
        previousMillis11 = currentMillis; 
      }

      if (isStopping11 && (currentMillis - previousMillis11 >= stopInterval)) {
        isStopping11 = false;
      }
      break;

    case 12:
      if (!isMoving12 && !isStopping12) {
        BACKWARD();                  
        isMoving12 = true;       
        actionMillis12 = currentMillis; 
      }

      if (isMoving12 && (currentMillis - actionMillis12 >= forwardDuration)) {
        STOP();
        isMoving12 = false;    
        isStopping12 = true;        
        previousMillis12 = currentMillis; 
      }

      if (isStopping12 && (currentMillis - previousMillis12 >= stopInterval)) {
        isStopping12 = false;
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
void FORWARD(){
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
