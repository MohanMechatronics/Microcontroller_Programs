#include <esp_now.h>
#include <WiFi.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define CHANNEL 0

const int motor1A = 27;
const int motor1B = 26;
const int motor2A = 25;
const int motor2B = 33;

#define I2C_SDA 14
#define I2C_SCL 13
TwoWire I2Cbus = TwoWire(0);

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels
// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
#define OLED_RESET     -1 // Reset pin # (or -1 if sharing Arduino reset pin)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &I2Cbus, OLED_RESET);

#include "eyes.h"
int xp=16;
int mood=0;

// Init ESP Now with fallback
void InitESPNow() {
  WiFi.disconnect();
  if (esp_now_init() == ESP_OK) {
    Serial.println("ESPNow Init Success");
  }
  else {
    Serial.println("ESPNow Init Failed");
    // Retry InitESPNow, add a counte and then restart?
    // InitESPNow();
    // or Simply Restart
    ESP.restart();
  }
}

// config AP SSID
void configDeviceAP() {
  const char *SSID = "Slave_5";
  bool result = WiFi.softAP(SSID, "Slave_1_Password", CHANNEL, 0);
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

  // configure LED PWM functionalitites
  ledcSetup(0,2000,8); //ledcSetup(ledChannel, freq 2 KHz , resolution);
  ledcSetup(1,2000,8);
  ledcSetup(2,2000,8);
  ledcSetup(3,2000,8);

  
  // attach the channel to the GPIO to be controlled
  ledcAttachPin(motor1A, 0); //ledcAttachPin(ledPin, ledChannel);
  ledcAttachPin(motor1B, 1);
  ledcAttachPin(motor2A, 2);
  ledcAttachPin(motor2B, 3);

  Serial.println("ESPNow/Basic/Slave Example");
  //Set device in AP mode to begin with
  WiFi.mode(WIFI_AP);
  // configure device AP mode
  configDeviceAP();
  // This is the mac address of the Slave in AP Mode
  Serial.print("AP MAC: "); Serial.println(WiFi.softAPmacAddress());
  // Init ESPNow with a fallback logic
  InitESPNow();
  // Once ESPNow is successfully Init, we will register for recv CB to
  // get recv packer info.
  esp_now_register_recv_cb(OnDataRecv);
}

// callback when data is recv from Master
void OnDataRecv(const uint8_t *mac_addr, const uint8_t *data, int data_len) {
  char macStr[18];
  snprintf(macStr, sizeof(macStr), "%02x:%02x:%02x:%02x:%02x:%02x",
           mac_addr[0], mac_addr[1], mac_addr[2], mac_addr[3], mac_addr[4], mac_addr[5]);
  Serial.print("Last Packet Recv from: "); Serial.println(macStr);
  Serial.print("Last Packet Recv Data: "); Serial.println(*data);
  Serial.println("");
  switch(*data)
  {
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
  ledcWrite (0, 255);
  ledcWrite(1, 0);
  ledcWrite(2, 255);
  ledcWrite(3, 0);
}
void BACKWARD(){
  // BACKWARD
  ledcWrite(0, 0);
  ledcWrite(1, 255);
  ledcWrite(2, 0);
  ledcWrite(3, 255);
}
void LEFT(){
  // LEFT
  ledcWrite(0, 0);
  ledcWrite(1, 255);
  ledcWrite(2, 255);
  ledcWrite(3, 0);
}
void RIGHT(){
  // RIGHT
  ledcWrite(0, 255);
  ledcWrite(1, 0);
  ledcWrite(2, 0);
  ledcWrite(3, 255);
}
void STOP(){
  // STOP
  ledcWrite(0, 0);
  ledcWrite(1, 0);
  ledcWrite(2, 0);
  ledcWrite(3, 0);
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