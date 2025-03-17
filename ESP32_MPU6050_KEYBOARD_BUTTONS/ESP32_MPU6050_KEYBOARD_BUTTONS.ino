 #include "I2Cdev.h"
#include "MPU6050_6Axis_MotionApps20.h"

MPU6050 mpu;
bool dmpReady = false;
uint8_t devStatus;
uint8_t fifoBuffer[64];
Quaternion q;
VectorFloat gravity;
float ypr[3];

// Variable to track the last printed direction
String lastDirection = "";

void setupMPU() {
  // Initialize Wire with custom SDA (26) and SCL (27)
  Wire.begin(26, 27); // SDA on pin 26, SCL on pin 27
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

    // Directly print the direction based on conditions
    if (yAxisValue < 75 && lastDirection != "Forward") {
      Serial.println("Forward");
      lastDirection = "Forward";
    } else if (xAxisValue < 75 && lastDirection != "Left") {
      Serial.println("Left");
      lastDirection = "Left";
    } else if (xAxisValue > 175 && lastDirection != "Right") {
      Serial.println("Right");
      lastDirection = "Right";
    } else if (yAxisValue > 175 && lastDirection != "Backward") {
      Serial.println("Backward");
      lastDirection = "Backward";
    }
    else if (yAxisValue > 75 && yAxisValue < 175 && xAxisValue > 75 && xAxisValue < 175 && lastDirection != "Stop") {
      Serial.println("Stop");
      lastDirection = "Stop";
    }

    delay(50);
  }
}
