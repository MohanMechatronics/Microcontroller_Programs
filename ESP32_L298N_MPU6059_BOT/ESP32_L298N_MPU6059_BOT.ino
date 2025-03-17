#include <Wire.h>
#include <MPU6050.h>

MPU6050 mpu;

// Timers
unsigned long timer = 0;
float timeStep = 0.01;

// Pitch, Roll and Yaw values
float pitch = 0;
float roll = 0;
float yaw = 0;

int ena = 25;
int enb = 13;
int ina = 27;
int inb = 26;
int inc = 12;
int ind = 14;

const int freq = 2000;
const int resolution = 8; // 8-bit resolution
const int channelA = 0;
const int channelB = 1;

void setup() 
{
  Serial.begin(115200);

  // Initialize MPU6050
  while(!mpu.begin(MPU6050_SCALE_2000DPS, MPU6050_RANGE_2G))
  {
    Serial.println("Could not find a valid MPU6050 sensor, check wiring!");
    delay(500);
  }
  
  // Calibrate gyroscope. The calibration must be at rest.
  // If you don't want calibrate, comment this line.
  mpu.calibrateGyro();

  // Set threshold sensitivity. Default 3.
  // If you don't want to use threshold, comment this line or set 0.
  mpu.setThreshold(3);

  pinMode(ina, OUTPUT);
  pinMode(inb, OUTPUT);
  pinMode(inc, OUTPUT);
  pinMode(ind, OUTPUT);

  // Setup LEDC channels
  ledcSetup(channelA, freq, resolution);
  ledcSetup(channelB, freq, resolution);

  // Attach channels to the respective pins
  ledcAttachPin(ena, channelA);
  ledcAttachPin(enb, channelB);
}

void loop()
{
  timer = millis();

  // Read normalized values
  Vector norm = mpu.readNormalizeGyro();

  // Calculate Pitch, Roll and Yaw
  pitch = pitch + norm.YAxis * timeStep;
  roll = roll + norm.XAxis * timeStep;
  yaw = yaw + norm.ZAxis * timeStep;

  // Output raw
  Serial.print(" Pitch = ");
  Serial.print(pitch);
  Serial.print(" Roll = ");
  Serial.print(roll);  
  Serial.print(" Yaw = ");
  Serial.println(yaw);

  // Wait to full timeStep period
  delay((timeStep * 1000) - (millis() - timer));

  if (yaw <= 3 && yaw >= -3) { // you can increase the range to suit your sensor's accuracy
    digitalWrite(ina, HIGH);
    digitalWrite(inb, HIGH);
    ledcWrite(channelA, 0);
    digitalWrite(inc, HIGH);
    digitalWrite(ind, HIGH);
    ledcWrite(channelB, 0);
    digitalWrite(led, HIGH);
  } else {
    if (yaw < -3) {
      digitalWrite(ina, HIGH);
      digitalWrite(inb, LOW);
      ledcWrite(channelA, 80);
      digitalWrite(inc, LOW);
      digitalWrite(ind, HIGH);
      ledcWrite(channelB, 80);
      digitalWrite(led, LOW);
    } else {
      digitalWrite(ina, LOW);
      digitalWrite(inb, HIGH);
      ledcWrite(channelA, 80);
      digitalWrite(inc, HIGH);
      digitalWrite(ind, LOW);
      ledcWrite(channelB, 80);
      digitalWrite(led, LOW);
    }
  }
}
