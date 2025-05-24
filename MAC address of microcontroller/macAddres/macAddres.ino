// /*
void setup(void) {
  Serial.begin(115200);
  Serial.println("-----------------");
  uint8_t macBT[6];
  esp_read_mac(macBT, ESP_MAC_BT);
}
void loop() {
  uint8_t macBT[6];
  esp_read_mac(macBT, ESP_MAC_BT);
  Serial.printf("%02X:%02X:%02X:%02X:%02X:%02X\r\n", macBT[0], macBT[1], macBT[2], macBT[3], macBT[4], macBT[5]);
  Serial.printf("0x%02X, 0x%02X, 0x%02X, 0x%02X, 0x%02X, 0x%02X\r\n", macBT[0], macBT[1], macBT[2], macBT[3], macBT[4], macBT[5]);
  delay(10000);  
}

// */

/*
// #include "WiFi.h"
#include <ESP8266WiFi.h>
void setup() {
  Serial.begin(115200);
  // WiFi.mode(WIFI_MODE_STA);
  WiFi.mode(WIFI_AP);
  Serial.println(WiFi.softAPmacAddress());
}
void loop() {
  
}
// */