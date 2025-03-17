#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>

// called this way, it uses the default address 0x40
Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver();

#define SERVOMIN  150 // This is the 'minimum' pulse length count (out of 4096)
#define SERVOMAX  600 // This is the 'maximum' pulse length count (out of 4096)
#define USMIN  600    // This is the rounded 'minimum' microsecond length based on the minimum pulse of 150
#define USMAX  2400   // This is the rounded 'maximum' microsecond length based on the maximum pulse of 600

int data;

void setup() {
  Serial.begin(9600);
  pinMode(8,OUTPUT);
  pinMode(9,OUTPUT);
  pinMode(10,OUTPUT);
  pinMode(11,OUTPUT);
  pwm.begin();
  pwm.setPWMFreq(60);  // Analog servos run at ~60 Hz updates
}

void setServoPulse(uint8_t n, double pulse) {
  double pulselength;
  pulselength = 1000000;   // 1,000,000 us per second
  pulselength /= 60;   // 60 Hz
  pulselength /= 4096;  // 12 bits of resolution
  pulse *= 1000;
  pulse /= pulselength;
  pwm.setPWM(n, 0, pulse);
}

void moveServos(int start, int end) {
  for (int pos = start; pos <= end; pos += 1) {
    int pulseLength = map(pos, 0, 180, SERVOMIN, SERVOMAX);
    pwm.setPWM(0, 0, pulseLength); // Servo 1
    pwm.setPWM(1, 0, pulseLength); // Servo 2
    delay(15);
  }
}

void loop() {
  if (Serial.available()) {
    data = Serial.read();

    if (data == 'M') {
      moveServos(0, 90);
    }
    if (data == 'N') {
      moveServos(90, 0);
    }
     if(data == 70)
    {
      Serial.println("FORWARD");
      digitalWrite(8, HIGH);
      digitalWrite(9, LOW);
      digitalWrite(10, HIGH);
      digitalWrite(11, LOW);
    }
 if(data == 66)
    {
      Serial.println("BACKWARD");
      digitalWrite(8, LOW);
      digitalWrite(9, HIGH);
      digitalWrite(10, LOW);
      digitalWrite(11, HIGH);
    }
    if(data == 76)
    {
      Serial.println("LEFT");
      digitalWrite(8, HIGH);
      digitalWrite(9, LOW);
      digitalWrite(10, LOW);
      digitalWrite(11, HIGH);
    }
  if(data == 82)
    {
      Serial.println("RIGHT");
      digitalWrite(8, LOW);
      digitalWrite(9, HIGH);
      digitalWrite(10, HIGH);
      digitalWrite(11, LOW);
    }
  if(data == 83)
    {
      Serial.println("STOP");
      digitalWrite(8, LOW);
      digitalWrite(9, LOW);
      digitalWrite(10, LOW);
      digitalWrite(11, LOW);
    }
  }
}
