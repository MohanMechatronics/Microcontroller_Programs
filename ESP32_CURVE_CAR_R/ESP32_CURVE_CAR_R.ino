#include <esp_now.h>
#include <WiFi.h>
#include <vector> // Include the vector header

#define CHANNEL 0

const int motor1A = 26;
const int motor1B = 25;
const int motor2A = 14;
const int motor2B = 27;

int SPEED = 255;
int speed_Coeff = 3;

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

// config AP SSID
void configDeviceAP() {
  const char *SSID = "Slave_1";
  bool result = WiFi.softAP(SSID, "Slave_1_Password", CHANNEL, 0);
  if (!result) {
    Serial.println("AP Config failed.");
  } else {
    Serial.println("AP Config Success. Broadcasting with AP: " + String(SSID));
  }
}

// callback when data is recv from Master
void OnDataRecv(const uint8_t *mac_addr, const uint8_t *data, int data_len) {
  char macStr[18];
  snprintf(macStr, sizeof(macStr), "%02x:%02x:%02x:%02x:%02x:%02x",
           mac_addr[0], mac_addr[1], mac_addr[2], mac_addr[3], mac_addr[4], mac_addr[5]);
  Serial.print("Last Packet Recv from: "); Serial.println(macStr);
  Serial.print("Last Packet Recv Data: "); Serial.println(*data);
  Serial.println("");
  if (*data == 1) Forward();  
  else if (*data == 2) Backward();
  else if (*data == 3) TurnRight();
  else if (*data == 4) TurnLeft();
  else if (*data == 5) ForwardLeft();
  else if (*data == 6) BackwardLeft();
  else if (*data == 7) ForwardRight();
  else if (*data == 8) BackwardRight();
  else if (*data == 0) Stop();
}

void setup() {
  Serial.begin(115200);

  // configure LED PWM functionalitites
  ledcSetup(0,2000,8); //ledcSetup(ledChannel, freq 2 KHz , resolution);
  ledcSetup(1,2000,8);
  ledcSetup(2,2000,8);
  ledcSetup(3,2000,8);

  // attach the channel to the GPIO to be controlled
  ledcAttachPin(motor1A, 0); //ledcAttachPin(ledPin, ledChannel);
  ledcAttachPin(motor1B, 1);
  ledcAttachPin(motor2A, 2);
  ledcAttachPin(motor2B, 3);

  Serial.println("ESPNow/Basic/Slave Example");
  //Set device in AP mode to begin with
  WiFi.mode(WIFI_AP);
  // configure device AP mode
  configDeviceAP();
  // This is the mac address of the Slave in AP Mode
  Serial.print("AP MAC: "); Serial.println(WiFi.softAPmacAddress());
  // Init ESPNow with a fallback logic
  InitESPNow();
  // Once ESPNow is successfully Init, we will register for recv CB to
  // get recv packer info.
  esp_now_register_recv_cb(OnDataRecv);

}

void  Forward(){
  //FORWARD
  ledcWrite (0, SPEED);
  ledcWrite(1, 0);
  ledcWrite(2, SPEED);
  ledcWrite(3, 0);
}
void Backward(){
  // BACKWARD
  ledcWrite(0, 0);
  ledcWrite(1, SPEED);
  ledcWrite(2, 0);
  ledcWrite(3, SPEED);
}
void TurnLeft(){
  // LEFT
  ledcWrite(0, 0);
  ledcWrite(1, SPEED);
  ledcWrite(2, SPEED);
  ledcWrite(3, 0);
}
void TurnRight(){
  // RIGHT
  ledcWrite(0, SPEED);
  ledcWrite(1, 0);
  ledcWrite(2, 0);
  ledcWrite(3, SPEED);
}

// function to move forward left
void ForwardLeft() {
  ledcWrite (0, SPEED);
  ledcWrite(1, 0);
  ledcWrite(2, SPEED / speed_Coeff);
  ledcWrite(3, 0);
}

// function to move backward left
void BackwardLeft() {
  ledcWrite(0, 0);
  ledcWrite(1, SPEED);
  ledcWrite(2, 0);
  ledcWrite(3, SPEED / speed_Coeff);
}

// function to move forward right
void ForwardRight() {
  ledcWrite (0, SPEED / speed_Coeff);
  ledcWrite(1, 0);
  ledcWrite(2, SPEED);
  ledcWrite(3, 0);
}

// function to move backward right
void BackwardRight() {
 ledcWrite(0, 0);
  ledcWrite(1, SPEED / speed_Coeff);
  ledcWrite(2, 0);
  ledcWrite(3, SPEED);
}
void Stop(){
  // STOP
  ledcWrite(0, 0);
  ledcWrite(1, 0);
  ledcWrite(2, 0);
  ledcWrite(3, 0);
}

void loop() {
  // Chill bro
}