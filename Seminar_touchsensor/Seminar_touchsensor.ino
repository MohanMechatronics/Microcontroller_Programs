#define TOUCH_PIN 26 // Pin connected to the touch sensor mode 0
#define TOUCH_PIN2 25 // Pin connected to the touch sensor mode B

#define RELAY_PIN 27 // Pin connected to the relay module

void setup() {
  pinMode(TOUCH_PIN, INPUT);
  pinMode(TOUCH_PIN2, INPUT);
  pinMode(RELAY_PIN, OUTPUT);

}

void loop() {
  int touchValue = digitalRead(TOUCH_PIN);
  int touchValue2 = digitalRead(TOUCH_PIN2);

  if (touchValue == HIGH) {

    digitalWrite(RELAY_PIN, HIGH);

  }else if (touchValue2 == HIGH) {

    digitalWrite(RELAY_PIN, HIGH);

  } else {
    digitalWrite(RELAY_PIN, LOW);
  }
}
