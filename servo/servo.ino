#include <ESP32Servo.h>

#define pin_servo 14

Servo myservo;

void setup(){
  // put your setup code here, to run once:
  myservo.attach(pin_servo);
}

void loop() {
  // put your main code here, to run repeatedly:
  myservo.write(0);
  delay(2000);
  myservo.write(90);
  delay(2000);
  myservo.write(180);
  delay(2000);

  /*
  myservo.write(90);
  delay(2000);
  myservo.write(180);
  delay(2000);
  myservo.write(90);
  delay(2000);
  */

}

