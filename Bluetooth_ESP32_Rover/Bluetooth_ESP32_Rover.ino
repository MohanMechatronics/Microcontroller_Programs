#include <Arduino.h>
#include "BluetoothSerial.h"
#include <vector>

BluetoothSerial SerialBT;

char t;
 
void setup() {
  Serial.begin(115200);

  pinMode(14,OUTPUT);   //left motors  forward
  pinMode(27,OUTPUT);   //left motors reverse
  pinMode(26,OUTPUT);   //right  motors forward
  pinMode(25,OUTPUT);   //right motors reverse
  pinMode(4,OUTPUT);   //Led

  SerialBT.begin("HAVE_FUN"); // Bluetooth device name

  // Wait for a connection from the joystick app
  Serial.println("Waiting for Bluetooth connection...");
  while (!SerialBT.connected())
  {
    delay(1000);
    Serial.println("Still waiting...");
  }
  Serial.println("Bluetooth connected!");
}
 
void loop() {
if (SerialBT.available()){
  t = SerialBT.read();
  Serial.println(t);
}
 
if(t == 'F'){            //move  forward(all motors rotate in forward direction)
  digitalWrite(14,HIGH);
  digitalWrite(25,HIGH);
}
 
else if(t == 'B'){      //move reverse (all  motors rotate in reverse direction)
  digitalWrite(27,HIGH);
  digitalWrite(26,HIGH);
}
  
else if(t == 'L'){      //turn right (left side motors rotate in forward direction,  right side motors doesn't rotate)
  digitalWrite(14,HIGH);
}
 
else  if(t == 'R'){      //turn left (right side motors rotate in forward direction, left  side motors doesn't rotate)
  digitalWrite(25,HIGH);
}

else if(t == 'C'){      // CLOCKWISE
  digitalWrite(14,HIGH);
  digitalWrite(26,HIGH);
}
 
else  if(t == 'A'){    // ANTI-CLOCKWISE  
  digitalWrite(25,HIGH);
  digitalWrite(27,HIGH);
}

else if(t ==  'W'){    //turn led on or off)
  digitalWrite(4,HIGH);
}
else if(t == 'w'){
  digitalWrite(4,LOW);
}
 
else if(t == 'S'){      //STOP (all motors stop)
  digitalWrite(14,LOW);
  digitalWrite(27,LOW);
  digitalWrite(25,LOW);
  digitalWrite(26,LOW);
}
delay(50);
}