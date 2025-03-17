#include <Arduino.h>
#include <WiFi.h>
#include <WebSocketsClient.h>

// Define WiFi credentials
const char* ssid = "SIMPLE_BOT";
const char* password = "mechatronics";

// WebSocket client
WebSocketsClient webSocket;

// Define button pins
#define BTN_FORWARD 14
#define BTN_BACKWARD 27
#define BTN_LEFT 26
#define BTN_RIGHT 25
#define LED_STATUS 2 // LED pin for WiFi connection status

// Button states
bool lastForwardState = false;
bool lastBackwardState = false;
bool lastLeftState = false;
bool lastRightState = false;

void sendWebSocketData(const String& data) {
  if (webSocket.isConnected()) {
    webSocket.sendTXT(data.c_str());  // Use c_str() to get a char* representation
    Serial.println("Sent: " + data); // Log the data being sent
  } else {
    Serial.println("WebSocket not connected");
  }
}

// WebSocket event handler
void webSocketEvent(WStype_t type, uint8_t* payload, size_t length) {
  switch (type) {
    case WStype_DISCONNECTED:
      Serial.println("WebSocket Disconnected");
      break;
    case WStype_CONNECTED:
      Serial.println("WebSocket Connected");
      break;
    case WStype_TEXT:
      Serial.printf("WebSocket Message: %s\n", payload);
      break;
    default:
      break;
  }
}

// Function to update LED status
void updateLEDStatus() {
  if (WiFi.status() == WL_CONNECTED) {
    digitalWrite(LED_STATUS, HIGH); // Turn on LED when connected
  } else {
    digitalWrite(LED_STATUS, LOW); // Turn off LED when disconnected
  }
}

void setup() {
  Serial.begin(115200);

  // Initialize button pins
  pinMode(BTN_FORWARD, INPUT_PULLUP);
  pinMode(BTN_BACKWARD, INPUT_PULLUP);
  pinMode(BTN_LEFT, INPUT_PULLUP);
  pinMode(BTN_RIGHT, INPUT_PULLUP);

  // Initialize LED pin
  pinMode(LED_STATUS, OUTPUT);
  digitalWrite(LED_STATUS, LOW); // Ensure LED is off initially

  // Connect to WiFi
  WiFi.begin(ssid, password);
  Serial.println("Connecting to WiFi...");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
    updateLEDStatus(); // Update LED during connection attempt
  }
  Serial.println("\nConnected to WiFi");
  updateLEDStatus(); // Ensure LED is updated after connection

  // Connect to WebSocket server
  webSocket.begin("192.168.4.1", 80, "/ws");
  webSocket.onEvent(webSocketEvent);

  Serial.println("WebSocket setup completed");
}

void loop() {
  webSocket.loop();

  // Update LED status for WiFi
  updateLEDStatus();

  // Read button states
  bool currentForwardState = !digitalRead(BTN_FORWARD);
  bool currentBackwardState = !digitalRead(BTN_BACKWARD);
  bool currentLeftState = !digitalRead(BTN_LEFT);
  bool currentRightState = !digitalRead(BTN_RIGHT);

  // Send WebSocket commands based on button presses
  if (currentForwardState && !lastForwardState) {
    Serial.println("Button Pressed: Forward");
    sendWebSocketData("1"); // Forward
  } else if (currentBackwardState && !lastBackwardState) {
    Serial.println("Button Pressed: Backward");
    sendWebSocketData("2"); // Backward
  } else if (currentLeftState && !lastLeftState) {
    Serial.println("Button Pressed: Left");
    sendWebSocketData("3"); // Left
  } else if (currentRightState && !lastRightState) {
    Serial.println("Button Pressed: Right");
    sendWebSocketData("4"); // Right
  }

  // Reset commands when buttons are released
  if ((!currentForwardState && lastForwardState) ||
      (!currentBackwardState && lastBackwardState) ||
      (!currentLeftState && lastLeftState) ||
      (!currentRightState && lastRightState)) {
    Serial.println("Button Released: Stop");
    sendWebSocketData("0"); // Stop
  }

  // Update button states
  lastForwardState = currentForwardState;
  lastBackwardState = currentBackwardState;
  lastLeftState = currentLeftState;
  lastRightState = currentRightState;

  delay(50); // Debounce delay
}
