#include <esp_now.h>
#include <WiFi.h>

#define BUTTON_PIN 0 // Button pin
#define LED_PIN 2    // LED pin

// Universal MAC Address (Broadcast)
uint8_t broadcastAddress[] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};

// Structure to send and receive data
typedef struct struct_message {
  int buttonState; // Button state: 1 (pressed), 0 (not pressed)
} struct_message;

// Create instances for sending and receiving data
struct_message myData;
struct_message incomingReadings;

// Global variables
volatile bool cumulativeState = false; // Tracks OR result across all devices
unsigned long lastResetTime = 0;       // Tracks time for resetting cumulative state
const unsigned long resetInterval = 100; // Interval to reset cumulative state in milliseconds

// Callback when data is sent
void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
  Serial.print("Send Status: ");
  Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Success" : "Fail");
}

// Callback when data is received
void OnDataRecv(const uint8_t *mac, const uint8_t *incomingData, int len) {
  memcpy(&incomingReadings, incomingData, sizeof(incomingReadings));
  Serial.printf("Received Button State: %d, Bytes: %d\n", incomingReadings.buttonState, len);

  // Update cumulative state with OR logic
  cumulativeState = cumulativeState || (incomingReadings.buttonState == 1);

  // Update LED based on cumulative state
  digitalWrite(LED_PIN, cumulativeState ? HIGH : LOW);
}

void setup() {
  // Initialize Serial Monitor
  Serial.begin(115200);

  // Configure pins
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW); // Ensure LED is off initially

  // Set device as a Wi-Fi Station
  WiFi.mode(WIFI_STA);

  // Init ESP-NOW
  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }

  // Register callbacks
  esp_now_register_send_cb(OnDataSent);
  esp_now_register_recv_cb(OnDataRecv);

  // Register peer
  esp_now_peer_info_t peerInfo = {};
  memcpy(peerInfo.peer_addr, broadcastAddress, 6);
  peerInfo.channel = 0;
  peerInfo.encrypt = false;

  if (esp_now_add_peer(&peerInfo) != ESP_OK) {
    Serial.println("Failed to add peer");
    return;
  }

  Serial.println("ESP-NOW Initialized");
}

void loop() {
  // Read the button state
  int buttonState = digitalRead(BUTTON_PIN) == LOW ? 1 : 0;

  // Set the button state to the struct
  myData.buttonState = buttonState;

  // Send the button state
  esp_err_t result = esp_now_send(broadcastAddress, (uint8_t *)&myData, sizeof(myData));
  if (result == ESP_OK) {
    Serial.println("Data sent successfully");
  } else {
    Serial.println("Error sending data");
  }

  // Reset cumulative state periodically
  if (millis() - lastResetTime > resetInterval) {
    cumulativeState = false; // Reset cumulative state
    lastResetTime = millis();
  }

  // Add a short delay to prevent overloading the loop
  delay(2);
}
