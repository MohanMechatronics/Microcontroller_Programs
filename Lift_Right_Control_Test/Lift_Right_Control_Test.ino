// Define output pins for motor control
const int motor1A = 14;
const int motor1B = 27;
const int motor2A = 26;
const int motor2B = 25;

void setup() {
  // configure LED PWM functionalitites
  ledcSetup(0,2000,8); //ledcSetup(ledChannel, freq, resolution);
  ledcSetup(1,2000,8);
  ledcSetup(2,2000,8);
  ledcSetup(3,2000,8);


  // attach the channel to the GPIO to be controlled
  ledcAttachPin(14, 0); //ledcAttachPin(ledPin, ledChannel);
  ledcAttachPin(27, 1);
  ledcAttachPin(26, 2);
  ledcAttachPin(25, 3);
}

void loop() {
  

// Move forward
  ledcWrite(0, 255);
  ledcWrite(1, 0);
  ledcWrite(2, 255);
  ledcWrite(3, 0);
  delay(2000); // 1 second

// Turn right
  ledcWrite(0, 150);
  ledcWrite(1, 0);
  ledcWrite(2, 0);
  ledcWrite(3, 150);
  delay(600); // 1 second
}
