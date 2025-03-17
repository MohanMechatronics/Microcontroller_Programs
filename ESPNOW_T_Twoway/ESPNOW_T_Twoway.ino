#include <esp_now.h>
#include <WiFi.h>

esp_now_peer_info_t slave;
#define CHANNEL 0
uint8_t data = 0;

void InitESPNow() {
  WiFi.disconnect();
  if (esp_now_init() == ESP_OK) {
    Serial.println("ESPNow Init Success");
  } else {
    Serial.println("ESPNow Init Failed");
    ESP.restart();
  }
}

void ScanForSlave() {
  uint8_t mac[6] = {0x08, 0xD1, 0xF9, 0x35, 0x47, 0x09}; // Replace with the slave MAC address
  memcpy(slave.peer_addr, mac, 6);
  slave.channel = CHANNEL;
  slave.encrypt = 0;

  if (!esp_now_is_peer_exist(slave.peer_addr)) {
    if (esp_now_add_peer(&slave) == ESP_OK) {
      Serial.println("Slave Added");
    } else {
      Serial.println("Failed to add slave");
    }
  }
}

void OnDataRecv(const uint8_t *mac_addr, const uint8_t *data, int len) {
  Serial.print("Data received: ");
  Serial.println(*data);
  digitalWrite(2, (*data == HIGH) ? LOW : HIGH);
}

void sendData() {
  data = digitalRead(0);
  Serial.print("Sending: ");
  Serial.println(data);
  esp_err_t result = esp_now_send(slave.peer_addr, &data, sizeof(data));
  if (result == ESP_OK) {
    Serial.println("Send success");
  } else {
    Serial.println("Send failed");
  }
}

void setup() {
  Serial.begin(115200);
  pinMode(0, INPUT);
  pinMode(2, OUTPUT);
  WiFi.mode(WIFI_AP_STA);
  InitESPNow();
  ScanForSlave();
  esp_now_register_recv_cb(OnDataRecv);
}

void loop() {
  sendData();
}
