const int buttonPin = 0;  // Pin connected to the button
bool buttonState = HIGH;  // Tracks the last button state
bool lastState = HIGH;

void setup() {
  Serial.begin(115200);
  pinMode(buttonPin, INPUT_PULLUP); // Using internal pull-up resistor
}

void loop() {
  buttonState = digitalRead(buttonPin);

  // Detect a state change (button press or release)
  if (buttonState != lastState) {
    if (buttonState == LOW) {
      Serial.println("forward"); // Send "forward" on button press
    } else {
      Serial.println("stop");    // Send "stop" on button release
    }
    lastState = buttonState; // Update the last state
  }

  delay(50); // Simple debounce
}
