#include <esp_now.h>
#include <WiFi.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

// Define pin numbers
#define UP_PIN 14
#define DOWN_PIN 27
#define LEFT_PIN 26
#define RIGHT_PIN 25

// Maximum number of devices
#define MAX_DEVICES 5

// Universal MAC Address (Broadcast)
uint8_t broadcastAddress[] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};

// Data structure for device data
typedef struct struct_message {
  char color[10];
  char numberPlate[15];
  int direction; // 0-Stop, 1-Up, 2-Down, 3-Left, 4-Right
} struct_message;

// Create instances for sending and receiving data
struct_message myData; // For controller's data
struct_message deviceData[MAX_DEVICES]; // Array to hold data for multiple devices
int deviceCount = 0; // Track connected devices

// LCD instance (I2C address 0x27, 16 columns, 2 rows)
LiquidCrystal_I2C lcd(0x27, 16, 2);

// Direction descriptions
const char *directions[] = {"S", "F", "B", "L", "R"};

// Callback when data is sent
void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
  Serial.print("Send Status: ");
  Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Success" : "Fail");
}

// Function to find or add a device by color
int findOrAddDevice(const char *color) {
  // Check if the device already exists
  for (int i = 0; i < deviceCount; i++) {
    if (strcmp(deviceData[i].color, color) == 0) {
      return i; // Return index if found
    }
  }
  
  // Add a new device if it doesn't exist
  if (deviceCount < MAX_DEVICES) {
    strcpy(deviceData[deviceCount].color, color);
    return deviceCount++;
  }
  
  return -1; // No space to add a new device
}

// Callback when data is received
void OnDataRecv(const uint8_t *mac, const uint8_t *incomingData, int len) {
  struct_message incomingReadings;
  memcpy(&incomingReadings, incomingData, sizeof(incomingReadings));

  Serial.printf("Received Data - Color: %s, Number Plate: %s, Direction: %d\n",
                incomingReadings.color, incomingReadings.numberPlate, incomingReadings.direction);

  // Find or add the device
  int index = findOrAddDevice(incomingReadings.color);
  if (index != -1) {
    // Update the corresponding device data
    strcpy(deviceData[index].numberPlate, incomingReadings.numberPlate);
    deviceData[index].direction = incomingReadings.direction;

    // Update the LCD
    updateLCD();
  }
}

// Function to update LCD display
void updateLCD() {
  lcd.clear(); // Clear LCD

  // Display information for each connected device
  for (int i = 0; i < deviceCount; i++) {
    lcd.setCursor(0, i); // Adjust for the row
    lcd.print(deviceData[i].color);
    lcd.print(": ");
    lcd.print(deviceData[i].numberPlate);
    lcd.print(" ");
    lcd.print(directions[deviceData[i].direction]);
  }
}

// Reset connection status if no data is received
void resetConnections() {
  deviceCount = 0;
  lcd.clear();
}

void setup() {
  Serial.begin(115200);

  lcd.init();
  lcd.backlight();
  lcd.clear();
  lcd.print("Initializing...");

  pinMode(UP_PIN, INPUT_PULLDOWN);
  pinMode(DOWN_PIN, INPUT_PULLDOWN);
  pinMode(LEFT_PIN, INPUT_PULLDOWN);
  pinMode(RIGHT_PIN, INPUT_PULLDOWN);

  WiFi.mode(WIFI_STA);

  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    lcd.setCursor(0, 1);
    lcd.print("ESP-NOW Failed");
    return;
  }

  esp_now_register_send_cb(OnDataSent);
  esp_now_register_recv_cb(OnDataRecv);

  esp_now_peer_info_t peerInfo = {};
  memcpy(peerInfo.peer_addr, broadcastAddress, 6);
  peerInfo.channel = 0;
  peerInfo.encrypt = false;

  if (esp_now_add_peer(&peerInfo) != ESP_OK) {
    Serial.println("Failed to add peer");
    lcd.setCursor(0, 1);
    lcd.print("Peer Add Failed");
    return;
  }

  Serial.println("ESP-NOW Initialized");
  lcd.clear();
  lcd.print("   WELCOME SIR");

  // Initialize  data
  strcpy(myData.color, "B");
  strcpy(myData.numberPlate, "TN01AB333");  // Example number plate
  myData.direction = 0; // Initial direction is stop
}

void loop() {
  if (digitalRead(UP_PIN) == HIGH) {
    myData.direction = 1; // Forward
  } else if (digitalRead(DOWN_PIN) == HIGH) {
    myData.direction = 2; // Backward
  } else if (digitalRead(LEFT_PIN) == HIGH) {
    myData.direction = 3; // Left
  } else if (digitalRead(RIGHT_PIN) == HIGH) {
    myData.direction = 4; // Right
  } else {
    myData.direction = 0; // Stop
  }

  esp_err_t result = esp_now_send(broadcastAddress, (uint8_t *)&myData, sizeof(myData));
  if (result == ESP_OK) {
    Serial.println("data sent successfully");
  } else {
    Serial.println("Error sending data");
  }

  static unsigned long lastReceiveTime = millis();
  if (millis() - lastReceiveTime > 5000) {
    resetConnections();
    lastReceiveTime = millis();
  }
}
