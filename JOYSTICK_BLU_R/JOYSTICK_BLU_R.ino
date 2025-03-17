#include "BluetoothSerial.h"

#if !defined(CONFIG_BT_ENABLED) || !defined(CONFIG_BLUEDROID_ENABLED)
#error Bluetooth is not enabled! Please run `make menuconfig` to and enable it
#endif

BluetoothSerial SerialBT;

void setup() {
  Serial.begin(115200);
  SerialBT.begin("ESP32_Allout"); //Bluetooth device name
  Serial.println("The device started, now you can pair it with bluetooth!");
}


void loop() {
  uint8_t recv_data[3];
  if (SerialBT.available()) {
    SerialBT.readBytes(recv_data, 3);
    
    if (recv_data[0] != 'T') {
      Serial.print("Receive error!");
      return;
    }
    
    Serial.printf("left_x: %d, left_y: %d\n", recv_data[1], recv_data[2]);
  }
}