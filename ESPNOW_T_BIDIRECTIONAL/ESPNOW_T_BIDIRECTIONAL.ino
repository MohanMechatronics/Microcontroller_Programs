#include <esp_now.h>
#include <WiFi.h>


uint8_t broadcastAddress[] = {0x08, 0xD1, 0xF9, 0x35, 0x47, 0x09};


int pushButtonStateOut; 
int pushButtonStateIn; 
String success; 

const int pushDown = 0; // GPIO2

const int lightME = 2; //GPIO4

typedef struct struct_message {
    bool state;
    
} struct_message;

// Create a struct_message called BME280Readings to hold sensor readings
struct_message stateIn;

// Create a struct_message to hold incoming sensor readings
struct_message StateOut;

esp_now_peer_info_t peerInfo;

// Callback when data is sent
void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
  Serial.print("\r\nLast Packet Send Status:\t");
  Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Delivery Success" : "Delivery Fail");
  if (status ==0){
    success = "Delivery Success :)";
  }
  else{
    success = "Delivery Fail :(";
  }
}

// Callback when data is received
void OnDataRecv(const uint8_t *mac_addr, const uint8_t *incomingData, int len) {
  memcpy(&stateIn, incomingData, sizeof(stateIn));
  Serial.print("Bytes received: ");
  Serial.println(len);

  
  pushButtonStateIn = stateIn.state;
  
 
}
 
void setup() {
  pinMode(pushDown,INPUT);
  pinMode(lightME, OUTPUT);
  // Init Serial Monitor
  Serial.begin(115200);

  // Set device as a Wi-Fi Station
  WiFi.mode(WIFI_STA);

  // Init ESP-NOW
  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }

  // Once ESPNow is successfully Init, we will register for Send CB to
  // get the status of Trasnmitted packet
  esp_now_register_send_cb(OnDataSent);
  
  // Register peer
  memcpy(peerInfo.peer_addr, broadcastAddress, 6);
  peerInfo.channel = 0;  
  peerInfo.encrypt = false;
  
  // Add peer        
  if (esp_now_add_peer(&peerInfo) != ESP_OK){
    Serial.println("Failed to add peer");
    return;
  }
  // Register for a callback function that will be called when data is received
  esp_now_register_recv_cb(OnDataRecv);
}
 
void loop() {
  getButtonReading();



  StateOut.state= pushButtonStateOut;

  
  
   // Send message via ESP-NOW
  esp_err_t result = esp_now_send(broadcastAddress, (uint8_t *) &StateOut, sizeof(StateOut));
  delay(50); // Add this delay
   
  if (result == ESP_OK) {
    Serial.println("Sent with success");
  }
  else {
    Serial.println("Error sending the data");
  }


  switchONorOFFled();
 
}



void getButtonReading(){
  pushButtonStateOut=digitalRead(pushDown);
}

void switchONorOFFled(){
 digitalWrite(lightME, pushButtonStateIn);
}