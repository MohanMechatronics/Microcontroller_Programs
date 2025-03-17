#include <ESP32Servo.h>

static const int servoPin = 26; // pin printed as G14 or 14
static const int potentiometerPin = 12;// pin printed as G12 or 12

static const int servoPin2 = 27;
static const int potentiometerPin2 = 14;

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

    int servoPosition_2 = map(analogRead(potentiometerPin2), 0, 4096, 0, 180);
    servo2.write(servoPosition_2);
    Serial.println(servoPosition_2);
    delay(20);
}