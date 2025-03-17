#include <ESP32Servo.h>

static const int servoPin = 12; // pin printed as G14 or 14
static const int potentiometerPin = 13;// pin printed as G12 or 12
static const int servoPin2 = 14; // pin printed as G14 or 14
static const int potentiometerPin2 = 27;// pin printed as G12 or 12

Servo servo1;
Servo servo2;

void setup() {
    Serial.begin(115200);
    servo1.attach(servoPin);
    servo2.attach(servoPin2);
}

void loop() {
    int servoPosition = map(analogRead(potentiometerPin), 0, 4096, 0, 180);
    servo1.write(servoPosition);
    Serial.println(servoPosition);
    delay(20);
    int servoPosition2 = map(analogRead(potentiometerPin2), 0, 4096, 0, 180);
    servo1.write(servoPosition2);
    Serial.println(servoPosition2);
    delay(20);
}