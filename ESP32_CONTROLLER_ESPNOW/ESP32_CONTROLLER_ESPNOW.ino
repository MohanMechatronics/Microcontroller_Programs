#include <esp_now.h>
#include <WiFi.h>

// Global copy of slave
esp_now_peer_info_t slave;

#define CHANNEL 0
#define PRINTSCANRESULTS 0
#define DELETEBEFOREPAIR 0

int mac[6] = {0xAC, 0x15, 0x18, 0xD5, 0xA0, 0x21}; //AC:15:18:D5:A0:21
// Define button pins
#define BTN_FORWARD 27
#define BTN_BACKWARD 26
#define BTN_LEFT 25
#define BTN_RIGHT 14
#define LED_STATUS 2 // LED pin for WiFi connection status

// Button states
bool lastForwardState = false;
bool lastBackwardState = false;
bool lastLeftState = false;
bool lastRightState = false;

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

// Scan for slaves in AP mode
void ScanForSlave() {
  //int8_t scanResults = WiFi.scanNetworks();
  // reset on each scan
  bool slaveFound = 0;
  memset(&slave, 0, sizeof(slave));

 
  for (int ii = 0; ii < 6; ++ii )
  {
    slave.peer_addr[ii] = (uint8_t) mac[ii];
  }

  slave.channel = CHANNEL; // pick a channel
  slave.encrypt = 0; // no encryption

  slaveFound = 1;



  if (slaveFound)
  {
    Serial.println("Slave Found, processing..");
  } else
  {
    Serial.println("Slave Not Found, trying again.");
  }

}

// Check if the slave is already paired with the master.
// If not, pair the slave with master
bool manageSlave() {
  if (slave.channel == CHANNEL) {
    if (DELETEBEFOREPAIR) {
      deletePeer();
    }

    Serial.print("Slave Status: ");
    // check if the peer exists
    bool exists = esp_now_is_peer_exist(slave.peer_addr);
    if ( exists) {
      // Slave already paired.
      Serial.println("Already Paired");
      return true;
    } else {
      // Slave not paired, attempt pair
      esp_err_t addStatus = esp_now_add_peer(&slave);
      if (addStatus == ESP_OK) {
        // Pair success
        Serial.println("Pair success");
        return true;
      } else if (addStatus == ESP_ERR_ESPNOW_NOT_INIT) {
        // How did we get so far!!
        Serial.println("ESPNOW Not Init");
        return false;
      } else if (addStatus == ESP_ERR_ESPNOW_ARG) {
        Serial.println("Invalid Argument");
        return false;
      } else if (addStatus == ESP_ERR_ESPNOW_FULL) {
        Serial.println("Peer list full");
        return false;
      } else if (addStatus == ESP_ERR_ESPNOW_NO_MEM) {
        Serial.println("Out of memory");
        return false;
      } else if (addStatus == ESP_ERR_ESPNOW_EXIST) {
        Serial.println("Peer Exists");
        return true;
      } else {
        Serial.println("Not sure what happened");
        return false;
      }
    }
  } else {
    // No slave found to process
    Serial.println("No Slave found to process");
    return false;
  }
}

void deletePeer() {
  esp_err_t delStatus = esp_now_del_peer(slave.peer_addr);
  Serial.print("Slave Delete Status: ");
  if (delStatus == ESP_OK) {
    // Delete success
    Serial.println("Success");
  } else if (delStatus == ESP_ERR_ESPNOW_NOT_INIT) {
    // How did we get so far!!
    Serial.println("ESPNOW Not Init");
  } else if (delStatus == ESP_ERR_ESPNOW_ARG) {
    Serial.println("Invalid Argument");
  } else if (delStatus == ESP_ERR_ESPNOW_NOT_FOUND) {
    Serial.println("Peer not found.");
  } else {
    Serial.println("Not sure what happened");
  }
}

uint8_t data = 0;
// send data
void sendData() {

  // Read button states
  bool currentForwardState = !digitalRead(BTN_FORWARD);
  bool currentBackwardState = !digitalRead(BTN_BACKWARD);
  bool currentLeftState = !digitalRead(BTN_LEFT);
  bool currentRightState = !digitalRead(BTN_RIGHT);

  if (currentForwardState && !lastForwardState) {
    Serial.println("Button Pressed: Forward");
    data = 1; // Forward
  } else if (currentBackwardState && !lastBackwardState) {
    Serial.println("Button Pressed: Backward");
    data = 2; // Backward
  } else if (currentLeftState && !lastLeftState) {
    Serial.println("Button Pressed: Left");
    data = 3; // Left
  } else if (currentRightState && !lastRightState) {
    Serial.println("Button Pressed: Right");
    data = 4; // Right
  }

  // Reset commands when buttons are released
  if ((!currentForwardState && lastForwardState) ||
      (!currentBackwardState && lastBackwardState) ||
      (!currentLeftState && lastLeftState) ||
      (!currentRightState && lastRightState)) {
    Serial.println("Button Released: Stop");
    data = 0; // Stop
  }

  // Update button states
  lastForwardState = currentForwardState;
  lastBackwardState = currentBackwardState;
  lastLeftState = currentLeftState;
  lastRightState = currentRightState;

  const uint8_t *peer_addr = slave.peer_addr;
  Serial.print("Sending: "); Serial.println(data);
  // Send message via ESP-NOW
  esp_err_t result = esp_now_send(peer_addr, (uint8_t *) &data, sizeof(data));

  Serial.print("Send Status: ");
  if (result == ESP_OK) {
    Serial.println("Success");
  } else if (result == ESP_ERR_ESPNOW_NOT_INIT) {
    // How did we get so far!!
    Serial.println("ESPNOW not Init.");
  } else if (result == ESP_ERR_ESPNOW_ARG) {
    Serial.println("Invalid Argument");
  } else if (result == ESP_ERR_ESPNOW_INTERNAL) {
    Serial.println("Internal Error");
  } else if (result == ESP_ERR_ESPNOW_NO_MEM) {
    Serial.println("ESP_ERR_ESPNOW_NO_MEM");
  } else if (result == ESP_ERR_ESPNOW_NOT_FOUND) {
    Serial.println("Peer not found.");
  } else {
    Serial.println("Not sure what happened");
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

  // Initialize button pins
  pinMode(BTN_FORWARD, INPUT_PULLUP);
  pinMode(BTN_BACKWARD, INPUT_PULLUP);
  pinMode(BTN_LEFT, INPUT_PULLUP);
  pinMode(BTN_RIGHT, INPUT_PULLUP);

  // Initialize LED pin
  pinMode(LED_STATUS, OUTPUT);
  digitalWrite(LED_STATUS, LOW); // Ensure LED is off initially

  //Set device in STA mode to begin with
  WiFi.mode(WIFI_STA);
  Serial.println("ESPNow/Basic/Master Example");
  // This is the mac address of the Master in Station Mode
  Serial.print("STA MAC: "); Serial.println(WiFi.macAddress());
  // Init ESPNow with a fallback logic
  InitESPNow();
  // Once ESPNow is successfully Init, we will register for Send CB to
  // get the status of Trasnmitted packet
  esp_now_register_send_cb(OnDataSent);
}

void loop() {
  // In the loop we scan for slave
  ScanForSlave();
  // If Slave is found, it would be populate in `slave` variable
  // We will check if `slave` is defined and then we proceed further
  if (slave.channel == CHANNEL) { // check if slave channel is defined
    // `slave` is defined
    // Add slave as peer if it has not been added already
    bool isPaired = manageSlave();
    if (isPaired) {
      // pair success or already paired
      // Send data to device
      sendData();
    } else {
      // slave pair failed
      Serial.println("Slave pair failed!");
    }
  }
  else {
    // No slave found to process
  }

  // wait for 3seconds to run the logic again

}