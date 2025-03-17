#include <ESP32Servo.h>

static const int servoPin = 12; // pin printed as G14 or 14
static const int potentiometerPin = 13;// pin printed as G12 or 12

static const int servoPin2 = 14; // pin printed as G14 or 14
static const int potentiometerPin2 = 27;// pin printed as G12 or 12

static const int servoPin3 = 12; 
static const int potentiometerPin3 = 13;

static const int servoPin4 = 14;
static const int potentiometerPin4 = 27;

static const int servoPin5 = 12; 
static const int potentiometerPin5 = 13;

static const int servoPin6 = 14;
static const int potentiometerPin6 = 27;

Servo servo1;
Servo servo2;
Servo servo3;
Servo servo4;
Servo servo5;
Servo servo6;

void setup() {
    Serial.begin(115200);
    servo1.attach(servoPin);
    servo2.attach(servoPin2);
    servo1.attach(servoPin3);
    servo2.attach(servoPin4);
    servo1.attach(servoPin5);
    servo2.attach(servoPin6);
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
    int servoPosition3 = map(analogRead(potentiometerPin3), 0, 4096, 0, 180);
    servo1.write(servoPosition3);
    Serial.println(servoPosition3);
    delay(20);
    int servoPosition4 = map(analogRead(potentiometerPin4), 0, 4096, 0, 180);
    servo1.write(servoPosition4);
    Serial.println(servoPosition4);
    delay(20);
    int servoPosition5 = map(analogRead(potentiometerPin5), 0, 4096, 0, 180);
    servo1.write(servoPosition5);
    Serial.println(servoPosition5);
    delay(20);
    int servoPosition6 = map(analogRead(potentiometerPin6), 0, 4096, 0, 180);
    servo1.write(servoPosition6);
    Serial.println(servoPosition6);
    delay(20);
}