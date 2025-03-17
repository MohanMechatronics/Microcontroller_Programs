#include <esp_now.h>
#include <WiFi.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// OLED setup (128x64, I2C address 0x3C)
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET    -1
#define SDA_PIN 21
#define SCL_PIN 22
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// Global copy of slave
esp_now_peer_info_t slave;

#define CHANNEL 0
#define PRINTSCANRESULTS 0
#define DELETEBEFOREPAIR 0

int mac[6] = {0x42, 0x91, 0x51, 0x59, 0x06, 0xA1}; // 42:91:51:59:06:A1

// Button Pins
#define BUTTON1_PIN 14
#define BUTTON2_PIN 27
#define BUTTON3_PIN 26

// Init ESP Now with fallback
void InitESPNow() {
  WiFi.disconnect();
  if (esp_now_init() == ESP_OK) {
    Serial.println("ESPNow Init Success");
  } else {
    Serial.println("ESPNow Init Failed");
    ESP.restart();
  }
}

// Scan for slaves in AP mode
void ScanForSlave() {
  bool slaveFound = 0;
  memset(&slave, 0, sizeof(slave));

  for (int ii = 0; ii < 6; ++ii) {
    slave.peer_addr[ii] = (uint8_t) mac[ii];
  }

  slave.channel = CHANNEL; // pick a channel
  slave.encrypt = 0;       // no encryption

  slaveFound = 1;

  if (slaveFound) {
    Serial.println("Slave Found, processing..");
  } else {
    Serial.println("Slave Not Found, trying again.");
  }
}

// Check if the slave is already paired with the master
bool manageSlave() {
  if (slave.channel == CHANNEL) {
    if (DELETEBEFOREPAIR) {
      esp_now_del_peer(slave.peer_addr);
    }

    bool exists = esp_now_is_peer_exist(slave.peer_addr);
    if (exists) {
      return true;
    } else {
      return esp_now_add_peer(&slave) == ESP_OK;
    }
  }
  return false;
}

uint8_t data = 0;

// Send data
void sendData(uint8_t value) {
  const uint8_t *peer_addr = slave.peer_addr;
  data = value;
  esp_err_t result = esp_now_send(peer_addr, (uint8_t *)&data, sizeof(data));
  Serial.print("Send Status: ");
  Serial.println(result == ESP_OK ? "Success" : "Failed");
}

void updateOLED(const char *line1, const char *line2) {
  display.clearDisplay();
  display.setTextSize(1.9);
  display.setTextColor(WHITE);
  display.setCursor(0, 10);
  display.println(line1);
  display.setCursor(0, 30);
  display.println(line2);
  display.display();
}

void setup() {
  Serial.begin(115200);

  pinMode(BUTTON1_PIN, INPUT);
  pinMode(BUTTON2_PIN, INPUT);
  pinMode(BUTTON3_PIN, INPUT);

  Wire.begin(SDA_PIN, SCL_PIN);
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println("SSD1306 allocation failed");
    while (1);
  }
  display.clearDisplay();
  updateOLED("Initializing...", "Please wait");

  WiFi.mode(WIFI_STA);
  InitESPNow();
  esp_now_register_send_cb([](const uint8_t *mac_addr, esp_now_send_status_t status) {
    Serial.print("Last Packet Send Status: ");
    Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Delivery Success" : "Delivery Fail");
  });
  updateOLED("Ready...", "Press a button");
}

void loop() {
  ScanForSlave();
  if (slave.channel == CHANNEL && manageSlave()) {
    if (digitalRead(BUTTON1_PIN) == HIGH) {
      sendData(1);
      updateOLED("HIGHWAY", "Speed limit - 100km/h");
    } else if (digitalRead(BUTTON2_PIN) == HIGH) {
      sendData(3);
      updateOLED("HOSPITAL ZONE", "Speed limit - 40km/h");
    } else if (digitalRead(BUTTON3_PIN) == HIGH) {
      sendData(2);
      updateOLED("SCHOOL ZONE", "Speed limit - 20km/h");
    } else {
      sendData(0);
      updateOLED("NORMAL ZONE", "Speed limit - 50km/h");
    }
  }
  delay(100);
}