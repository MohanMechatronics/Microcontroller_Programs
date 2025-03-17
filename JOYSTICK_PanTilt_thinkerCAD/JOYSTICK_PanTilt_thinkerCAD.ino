// C++ code
//
#include <Servo.h>

Servo servo_3;

Servo servo_4;

void setup()
{
  pinMode(A0, INPUT);
  pinMode(A1, INPUT);
    
  servo_3.attach(3, 500, 2500);
  servo_4.attach(5, 500, 2500);
}

void loop()
{
    
  servo_3.write(map(analogRead(A0), 0, 1023, 0, 180));
  servo_4.write(map(analogRead(A1), 0, 1023, 0, 180));
  delay(10); // Delay a little bit to improve simulation performance
}