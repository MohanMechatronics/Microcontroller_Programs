#include <Arduino.h>
#include "BluetoothSerial.h"
#include <vector>

BluetoothSerial SerialBT;

// Motor control definitions
#define UP 1
#define DOWN 2
#define TURN_LEFT 3
#define TURN_RIGHT 4
#define STOP 0

#define LEFT_MOTOR 0
#define RIGHT_MOTOR 1

#define FORWARD 1
#define BACKWARD -1

#define LED_PIN 2 // Define LED pin

struct MOTOR_PINS {
  int pinIN1;
  int pinIN2;    
};

std::vector<MOTOR_PINS> motorPins = {
  {13, 27},  // LEFT_MOTOR
  {26, 25},  // RIGHT_MOTOR 
};

// Function to rotate a specific motor
void rotateMotor(int motorNumber, int motorDirection) {
  if (motorDirection == FORWARD) {
    digitalWrite(motorPins[motorNumber].pinIN1, HIGH);
    digitalWrite(motorPins[motorNumber].pinIN2, LOW);    
  } else if (motorDirection == BACKWARD) {
    digitalWrite(motorPins[motorNumber].pinIN1, LOW);
    digitalWrite(motorPins[motorNumber].pinIN2, HIGH);     
  } else {
    digitalWrite(motorPins[motorNumber].pinIN1, LOW);
    digitalWrite(motorPins[motorNumber].pinIN2, LOW);       
  }
}

// Function to process movement commands
void processCarMovement(int inputValue) {
  switch (inputValue) {
    case UP:
      rotateMotor(RIGHT_MOTOR, FORWARD);
      rotateMotor(LEFT_MOTOR, FORWARD);
      Serial.println("UP");
      break;
    case DOWN:
      rotateMotor(RIGHT_MOTOR, BACKWARD);
      rotateMotor(LEFT_MOTOR, BACKWARD);
      Serial.println("DOWN");
      break;
    case TURN_LEFT:
      rotateMotor(LEFT_MOTOR, FORWARD);
      rotateMotor(RIGHT_MOTOR, BACKWARD);
      Serial.println("LEFT");
      break;
    case TURN_RIGHT:
      rotateMotor(RIGHT_MOTOR, FORWARD);
      rotateMotor(LEFT_MOTOR, BACKWARD);
      Serial.println("RIGHT");
      break;
    case STOP:
      rotateMotor(RIGHT_MOTOR, STOP);
      rotateMotor(LEFT_MOTOR, STOP);
      Serial.println("STOP");
      break;
    default:
      rotateMotor(RIGHT_MOTOR, STOP);
      rotateMotor(LEFT_MOTOR, STOP);
      break;
  }
}

// Function to initialize pin modes
void setUpPinModes() {
  for (int i = 0; i < motorPins.size(); i++) {
    pinMode(motorPins[i].pinIN1, OUTPUT);
    pinMode(motorPins[i].pinIN2, OUTPUT);
    rotateMotor(i, STOP);
  }
  pinMode(LED_PIN, OUTPUT); // Set LED pin as OUTPUT
  digitalWrite(LED_PIN, LOW); // Ensure LED is off initially
}

void setup() {
  setUpPinModes();
  Serial.begin(115200);
  SerialBT.begin("ROHIT_CAR"); // Bluetooth device name
  Serial.println("Bluetooth initialized.");
}

void handleCommand(char command) {
  if (command == 'F') { // Forward
    processCarMovement(UP);
  } else if (command == 'B') { // Backward
    processCarMovement(DOWN);
  } else if (command == 'L') { // Turn Left
    processCarMovement(TURN_LEFT);
  } else if (command == 'R') { // Turn Right
    processCarMovement(TURN_RIGHT);
  } else if (command == 'S') { // Stop
    processCarMovement(STOP);
  } else {
    processCarMovement(STOP); // Default to STOP for unknown commands
  }
}


void loop() {
  // Check Bluetooth connection and control LED
  if (SerialBT.hasClient()) {
    digitalWrite(LED_PIN, HIGH); // Turn on LED if connected
  } else {
    digitalWrite(LED_PIN, LOW); // Turn off LED if disconnected
  }

  // Handle incoming Bluetooth commands
  if (SerialBT.available()) {
    char command = SerialBT.read(); // Read single character
    if (command == 'F' || command == 'B' || command == 'L' || command == 'R' || command == 'S') {
      Serial.printf("Received Command: %c\n", command);
      handleCommand(command);
    } else {
      // Ignore invalid commands
      Serial.printf("Invalid Command: %c\n", command);
    }
  }
}

