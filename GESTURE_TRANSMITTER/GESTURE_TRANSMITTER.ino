#include <BluetoothSerial.h>
#include "I2Cdev.h"
#include "MPU6050_6Axis_MotionApps20.h"

BluetoothSerial SerialBT;

String MACadd = "CC:7B:5C:1E:F6:82"; // Robot's MAC address
uint8_t address[6] = {0xCC, 0x7B, 0x5C, 0x1E, 0xF6, 0x82}; // 

// MPU control/status vars
MPU6050 mpu;
bool dmpReady = false;
uint8_t devStatus;
uint8_t fifoBuffer[64];
Quaternion q;
VectorFloat gravity;
float ypr[3];

void setupMPU() {
  Wire.begin(); // For ESP32
  Wire.setClock(400000); // 400kHz I2C clock. Comment this line if having compilation difficulties

  mpu.initialize();
  devStatus = mpu.dmpInitialize();
  if (devStatus == 0) {
    mpu.CalibrateAccel(6);
    mpu.CalibrateGyro(6);
    mpu.setDMPEnabled(true);
    dmpReady = true;
  }
}

void setup() {
  Serial.begin(115200);
  SerialBT.begin("ESP32_MPU6050", true);
  Serial.println("The device started in master mode, make sure the remote BT device is on!");
  
  if (SerialBT.connect(address)) {
    Serial.println("Connected Successfully!");
  } else {
    while (!SerialBT.connected(10000)) {
      Serial.println("Failed to connect. Make sure the remote device is available and in range, then restart the app.");
    }
  }
  
  setupMPU();
}

void loop() {
  if (!dmpReady) {
    Serial.println("MPU initialization failed. Check your connections and try again.");
    delay(1000); // Wait for a moment before retrying
    return;
  }

  if (mpu.dmpGetCurrentFIFOPacket(fifoBuffer)) {
    mpu.dmpGetQuaternion(&q, fifoBuffer);
    mpu.dmpGetGravity(&gravity, &q);
    mpu.dmpGetYawPitchRoll(ypr, &q, &gravity);

    int xAxisValue = constrain(ypr[2] * 180 / M_PI, -90, 90);
    int yAxisValue = constrain(ypr[1] * 180 / M_PI, -90, 90);

    xAxisValue = map(xAxisValue, -90, 90, 0, 254);
    yAxisValue = map(yAxisValue, -90, 90, 0, 254);

    char command = 'S'; // Default to STOP

    if (yAxisValue < 75) {
      command = 'F'; // Forward
    } else if (xAxisValue > 175) {
      command = 'L'; // Turn Left
    } else if (xAxisValue < 75) {
      command = 'R'; // Turn Right
    } else if (yAxisValue > 175) {
      command = 'B'; // Backward
    } else if (yAxisValue == 80) {
      command = 'S'; // Stop
    }

    Serial.print("Command: ");
    Serial.println(command);

    SerialBT.write(command); // Send the command over Bluetooth
    delay(50);
  }
}
