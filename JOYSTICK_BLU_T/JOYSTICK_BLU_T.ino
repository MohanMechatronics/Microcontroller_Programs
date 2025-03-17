#include "BluetoothSerial.h"

#define Left_VRX_PIN 25
#define Left_VRY_PIN 26

int left_x_val;
int left_y_val;

BluetoothSerial SerialBT;

String MACadd = "A0:B7:65:63:8C:D2"; // Write Robot's MAC address
uint8_t address[6] = {0xA0, 0xB7, 0x65, 0x63, 0x8C, 0xD2};//Write Drone side MAC address in HEX
bool connected;

void setup() {
  Serial.begin(115200);
  SerialBT.begin("ESP32test", true); 
  Serial.println("The device started in master mode, make sure remote BT device is on!");
  
  // connect(address) is fast (upto 10 secs max), connect(name) is slow (upto 30 secs max) as it needs
  // to resolve name to address first, but it allows to connect to different devices with the same name.
  // Set CoreDebugLevel to Info to view devices bluetooth address and device names
  connected = SerialBT.connect(address);
  
  if(connected) {
    Serial.println("Connected Succesfully!");
  } else {
    while(!SerialBT.connected(10000)) {
      Serial.println("Failed to connect. Make sure remote device is available and in range, then restart app."); 
    }
  }
  // disconnect() may take upto 10 secs max
  if (SerialBT.disconnect()) {
    Serial.println("Disconnected Succesfully!");
  }
  // this would reconnect to the name(will use address, if resolved) or address used with connect(name/address).
  SerialBT.connect();

  pinMode(Left_VRX_PIN, INPUT);
  pinMode(Left_VRY_PIN, INPUT);
  
}


void loop() {

  uint8_t send_data[3];
  
  left_x_val = analogRead(Left_VRX_PIN) >> 4;//value 0-255 (">> 4" convert maximum value from 4095 to 255 )
  left_y_val = analogRead(Left_VRY_PIN) >> 4;//value 0-255  

  send_data[0] = 'T';
  send_data[1] = left_x_val;
  send_data[2] = left_y_val;
  // Print each element of the array individually
  for (int i = 0; i < 3; i++) {
    Serial.print(send_data[i]);  // Print the current element
    Serial.print(" ");           // Add a space between elements for clarity
  }
  Serial.println();  // Print a newline character to end the line
  
  SerialBT.write(send_data, 3);
  delay(20);
}