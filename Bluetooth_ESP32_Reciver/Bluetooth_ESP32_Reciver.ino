#include "BluetoothSerial.h"

BluetoothSerial SerialBT;

int Value;
int Value1;


void setup(void) 
{
  Serial.begin(115200);
  
  pinMode(2, OUTPUT);// BUILTIN LED PIN

  SerialBT.begin("RECIVER"); // Bluetooth device name

  // Wait for a connection from the joystick app
  Serial.println("Waiting for Bluetooth connection...");
  while (!SerialBT.connected())
  {
    delay(1000);
    Serial.println("Still waiting...");
  }
  Serial.println("Bluetooth connected!");

  pinMode(2, OUTPUT);
  
}

void loop() 
{
  uint8_t recv_data[2];
  if (SerialBT.available()) {
    SerialBT.readBytes(recv_data, 2);

    if (recv_data[0] != 'T') {
      Serial.print("Receive error!");
      return;
    }

    Value = recv_data[0];
    Value1 = recv_data[1]; // Joystick X-axis value
    Serial.printf("Value: %d, Value1: %d\n", recv_data[0], recv_data[1]);
    if(Value1 == 1){
      digitalWrite(2, LOW);
    }
    else{
      digitalWrite(2, HIGH);
    }
  }
}
