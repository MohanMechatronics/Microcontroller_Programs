#include "BluetoothSerial.h"

#define Joystick_X_PIN 33 // Replace with your joystick X-axis pin
#define Joystick_Y_PIN 32 // Replace with your joystick Y-axis pin

int joystick_x_val;
int joystick_y_val;
BluetoothSerial SerialBT;

String MACadd = "EC:62:60:32:7D:22"; // Write Robot's MAC address
uint8_t address[6] = {0xEC, 0x62, 0x60, 0x32, 0x7D, 0x22}; // Write Robot's MAC address in HEX
bool connected;

void setup() {
  Serial.begin(115200);
  SerialBT.begin("JoystickController", true);
  Serial.println("Joystick Controller started, make sure the robot is ready!");

  connected = SerialBT.connect(address);

  if (connected) {
    Serial.println("Connected Successfully!");
  } else {
    while (!SerialBT.connected(10000)) {
      Serial.println("Failed to connect. Make sure the robot is available and in range, then restart the app.");
    }
  }

  pinMode(Joystick_X_PIN, INPUT);
  pinMode(Joystick_Y_PIN, INPUT);
}

void loop() {
  uint8_t send_data[2];

  // Read joystick values
  joystick_x_val = analogRead(Joystick_X_PIN) >> 4; // value 0-255 (">> 4" convert maximum value from 4095 to 255)
  joystick_y_val = analogRead(Joystick_Y_PIN) >> 4; // value 0-255

  // Map joystick values to control commands
  char command = mapJoystickToCommand(joystick_x_val, joystick_y_val);

  send_data[1] = command;

  // Serial print the send_data
  Serial.print("Sending data: ");
  Serial.print(send_data[1]);
  Serial.println(); // Add a newline for clarity

  SerialBT.write(send_data, 1); // Send only 1 bytes

  delay(20);
}

char mapJoystickToCommand(int x_val, int y_val) {
  // Define joystick value thresholds for each direction
  int threshold = 100; // Adjust this threshold as needed

  if (y_val < (255 - threshold) && x_val >= (128 - threshold) && x_val <= (128 + threshold)) {
    return 'U'; // Up
  } else if (y_val > threshold && x_val >= (128 - threshold) && x_val <= (128 + threshold)) {
    return 'D'; // Down
  } else if (x_val < (128 - threshold) && y_val >= (128 - threshold) && y_val <= (128 + threshold)) {
    return 'L'; // Left
  } else if (x_val > (128 + threshold) && y_val >= (128 - threshold) && y_val <= (128 + threshold)) {
    return 'R'; // Right
  } else if (x_val < (128 - threshold) && y_val < (128 - threshold)) {
    return 'UL'; // Up-Left
  } else if (x_val > (128 + threshold) && y_val < (128 - threshold)) {
    return 'UR'; // Up-Right
  } else if (x_val < (128 - threshold) && y_val > (128 + threshold)) {
    return 'DL'; // Down-Left
  } else if (x_val > (128 + threshold) && y_val > (128 + threshold)) {
    return 'DR'; // Down-Right
  } else {
    return 'S'; // Stop (default)
  }
}
