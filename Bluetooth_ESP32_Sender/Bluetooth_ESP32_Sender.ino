#include "BluetoothSerial.h"

BluetoothSerial SerialBT;

int BUTTON;
int VALUE;

String MACadd = "A8:42:E3:AB:B6:7E"; // Write Robot's MAC address
uint8_t address[6] = {0xA8, 0x42, 0xE3, 0xAB, 0xB6, 0x7E}; // Write Robot's MAC address in HEX
bool connected;

void setup() {
  Serial.begin(115200);
  SerialBT.begin("Controller", true);
  Serial.println("Controller started, make sure the DEVICE is ready!");

  connected = SerialBT.connect(address);

  if (connected) {
    Serial.println("Connected Successfully!");
  } else {
    while (!SerialBT.connected(10000)) {
      Serial.println("Failed to connect. Make sure the DEVICE is available and in range, then restart the MCU.");
    }
  }
  // disconnect() may take upto 10 secs max
  if (SerialBT.disconnect()) {
    Serial.println("Disconnected Succesfully!");
  }
  // this would reconnect to the name(will use address, if resolved) or address used with connect(name/address).
  SerialBT.connect();

  pinMode(0, INPUT);
  pinMode(2, OUTPUT);
}

void loop() {
  uint8_t send_data[2];

  BUTTON = digitalRead(0);
  send_data[0] = 'T';
  send_data[1] = BUTTON;

  if(BUTTON == HIGH){
    digitalWrite(2, LOW);
  }
  else{
    digitalWrite(2, HIGH);
  }

  for (int i = 0; i < 2; i++) {
    Serial.print(send_data[i]);  // Print the current element
    Serial.print(" ");           // Add a space between elements for clarity
  }
  Serial.println();  // Print a newline character to end the line
  
  SerialBT.write(send_data, 2);
  delay(20);
}

