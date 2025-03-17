#include <esp_now.h>
#include <WiFi.h>

// Global copies of slaves
esp_now_peer_info_t slave1;
esp_now_peer_info_t slave2;

#define CHANNEL 0
#define PRINTSCANRESULTS 0
#define DELETEBEFOREPAIR 0

int mac1[6] = {0xA8, 0x42, 0xE3, 0xAB, 0xB6, 0x7D}; // A8:42:E3:AB:B6:7D
int mac2[6] = {0xD4, 0x8A, 0xFC, 0x9E, 0x83, 0xE1}; // D4:8A:FC:9E:83:E1

// Init ESP Now with fallback
void InitESPNow() {
  WiFi.disconnect();
  if (esp_now_init() == ESP_OK) {
    Serial.println("ESPNow Init Success");
  }
  else {
    Serial.println("ESPNow Init Failed");
    ESP.restart(); // Restart ESP if initialization fails
  }
}

// Scan for slaves in AP mode
void ScanForSlave() {
  bool slave1Found = false;
  bool slave2Found = false;

  memset(&slave1, 0, sizeof(slave1));
  memset(&slave2, 0, sizeof(slave2));

  for (int ii = 0; ii < 6; ++ii) {
    slave1.peer_addr[ii] = (uint8_t) mac1[ii];
    slave2.peer_addr[ii] = (uint8_t) mac2[ii];
  }

  slave1.channel = CHANNEL;
  slave1.encrypt = 0;

  slave2.channel = CHANNEL;
  slave2.encrypt = 0;

  slave1Found = true;
  slave2Found = true;

  if (slave1Found && slave2Found) {
    Serial.println("Both Slaves Found, processing..");
  } else if (slave1Found || slave2Found) {
    Serial.println("Only one Slave Found, processing..");
  } else {
    Serial.println("No Slaves Found, trying again.");
  }
}

// Check if the slave is already paired with the master.
// If not, pair the slave with master
bool manageSlave(esp_now_peer_info_t &slave) {
  if (slave.channel == CHANNEL) {
    if (DELETEBEFOREPAIR) {
      deletePeer(slave);
    }

    Serial.print("Slave Status: ");
    bool exists = esp_now_is_peer_exist(slave.peer_addr);
    if (exists) {
      Serial.println("Already Paired");
      return true;
    } else {
      esp_err_t addStatus = esp_now_add_peer(&slave);
      if (addStatus == ESP_OK) {
        Serial.println("Pair success");
        return true;
      } else {
        Serial.println("Pair failed");
        return false;
      }
    }
  } else {
    Serial.println("No Slave found to process");
    return false;
  }
}

void deletePeer(esp_now_peer_info_t &slave) {
  esp_err_t delStatus = esp_now_del_peer(slave.peer_addr);
  Serial.print("Slave Delete Status: ");
  if (delStatus == ESP_OK) {
    Serial.println("Success");
  } else {
    Serial.println("Failed");
  }
}

uint8_t data = 0;

// send data to both slaves
void sendDataToSlaves() {
  sendData(slave1);
  sendData(slave2);
}

// send data
void sendData(esp_now_peer_info_t &slave) {
  data = digitalRead(0);
  if (data == HIGH) {
    digitalWrite(2, LOW);
  } else {
    digitalWrite(2, HIGH);
  }

  const uint8_t *peer_addr = slave.peer_addr;
  Serial.print("Sending to Slave: "); Serial.println(peer_addr[5], HEX);
  Serial.print("Data: "); Serial.println(data);
  esp_err_t result = esp_now_send(peer_addr, (uint8_t *)&data, sizeof(data));
  Serial.print("Send Status: ");
  if (result == ESP_OK) {
    Serial.println("Success");
  } else {
    Serial.println("Failed");
  }
}

// callback when data is sent from Master to Slave
void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
  char macStr[18];
  snprintf(macStr, sizeof(macStr), "%02x:%02x:%02x:%02x:%02x:%02x",
           mac_addr[0], mac_addr[1], mac_addr[2], mac_addr[3], mac_addr[4], mac_addr[5]);
  Serial.print("Last Packet Sent to: "); Serial.println(macStr);
  Serial.print("Last Packet Send Status: "); Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Delivery Success" : "Delivery Fail");
}

void setup() {
  Serial.begin(115200);
  pinMode(0, INPUT);
  pinMode(2, OUTPUT);
  WiFi.mode(WIFI_STA);
  Serial.println("ESPNow/Basic/Master Example");
  Serial.print("STA MAC: "); Serial.println(WiFi.macAddress());
  InitESPNow();
  esp_now_register_send_cb(OnDataSent);
}

void loop() {
  ScanForSlave();
  if (slave1.channel == CHANNEL) {
    if (manageSlave(slave1)) {
      sendData(slave1);
    }
  }

  if (slave2.channel == CHANNEL) {
    if (manageSlave(slave2)) {
      sendData(slave2);
    }
  }
}
