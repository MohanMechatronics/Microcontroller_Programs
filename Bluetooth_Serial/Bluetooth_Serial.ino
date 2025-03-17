#include <Arduino.h>
#include "BluetoothSerial.h"
#include <vector>

#define LED_Blue    4

BluetoothSerial SerialBT;


void process(String inputValue)
{
  Serial.printf("Got value as %s %d\n", inputValue.c_str(), inputValue.toInt());  
  switch(inputValue.toInt())
  {
    case 1:
      digitalWrite(LED_Blue, HIGH);
      break;
    
    case 2:
      digitalWrite(LED_Blue, LOW);
      break;

    default:
      digitalWrite(LED_Blue, LOW);
      break;
  }
}

void setup(void) 
{
  Serial.begin(115200);
  pinMode(LED_Blue, OUTPUT);

  SerialBT.begin("HAVE_FUN"); // Bluetooth device name

  // Wait for a connection
  Serial.println("Waiting for Bluetooth connection...");
  while (!SerialBT.connected())
  {
    delay(1000);
    Serial.println("Still waiting...");
  }
  Serial.println("Bluetooth connected!");
}

void loop() 
{
  // Handle Bluetooth control
  if (SerialBT.available())
  {
    String inputValue = SerialBT.readStringUntil('\n');
    process(inputValue);
    Serial.println(inputValue);
  }
  // Continue with your other code as needed
}
