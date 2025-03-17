#include <ESP8266WiFi.h>
#include <espnow.h>

// Motor control pins
#define IN1 D5
#define IN2 D6
#define IN3 D7
#define IN4 D8

void setup() {
  Serial.begin(115200);

  // Configure motor pins as outputs
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);

  Serial.println("ESPNow/Basic/Slave Example");

  // Set device in AP mode
  WiFi.mode(WIFI_AP);
  configDeviceAP();

  // Initialize ESP-NOW
  InitESPNow();

  // Register callback for received data
  esp_now_set_self_role(ESP_NOW_ROLE_SLAVE);
  esp_now_register_recv_cb(OnDataRecv);
}

// Initialize ESP-NOW
void InitESPNow() {
  if (esp_now_init() == 0) {
    Serial.println("ESPNow Init Success");
  } else {
    Serial.println("ESPNow Init Failed. Restarting...");
    ESP.restart();
  }
}

// Configure device as AP
void configDeviceAP() {
  const char *SSID = "Slave_1";
  bool result = WiFi.softAP(SSID, "Slave_1_Password", 1, 0);
  if (!result) {
    Serial.println("AP Config failed.");
  } else {
    Serial.println("AP Config Success. Broadcasting with SSID: " + String(SSID));
  }
}

// Callback when data is received
void OnDataRecv(uint8_t *mac_addr, uint8_t *data, uint8_t data_len) {
  Serial.print("Data received: ");
  Serial.println(*data);

  // Control motor speed based on received data
  int speed;
  if (*data == 1) {
    speed = 255; // Full speed
  } else if (*data == 2) {
    speed = 100; // Medium speed
  } else if (*data == 3) {
    speed = 90; // Low speed
  } else  if (*data == 0){
    speed = 150; // Stop
  }else{
    speed = 150; // Stop
  }

  // Forward direction (set IN1/IN3 HIGH, IN2/IN4 LOW)
  analogWrite(IN1, speed);
  digitalWrite(IN2, LOW);
  analogWrite(IN3, speed);
  digitalWrite(IN4, LOW);

  Serial.print("Motor speed set to: ");
  Serial.println(speed);
}

void loop() {
  // Idle loop
}
