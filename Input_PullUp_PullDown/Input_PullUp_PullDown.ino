const int pullUpSwitchPin = 2;  // Pin for the pull-up switch
const int pullDownSwitchPin = 3; // Pin for the pull-down switch
const int builtInLedPin = 13;   // Built-in LED pin
const int externalLedPin = 8;   // Pin for the external LED

void setup() {
  // Configure the pins
  pinMode(pullUpSwitchPin, INPUT_PULLUP); // Pull-up switch
  pinMode(pullDownSwitchPin, INPUT);     // Pull-down switch
  pinMode(builtInLedPin, OUTPUT);        // Built-in LED
  pinMode(externalLedPin, OUTPUT);       // External LED
}

void loop() {
  // Read states of the switches
  int pullUpSwitchState = digitalRead(pullUpSwitchPin);    // LOW when pressed
  int pullDownSwitchState = digitalRead(pullDownSwitchPin); // HIGH when pressed

  // Control the built-in LED with the pull-up switch
  if (pullUpSwitchState == LOW) {  // Pull-up switch pressed
    digitalWrite(builtInLedPin, HIGH);
  } else {
    digitalWrite(builtInLedPin, LOW);
  }

  // Control the external LED with the pull-down switch
  if (pullDownSwitchState == HIGH) { // Pull-down switch pressed
    digitalWrite(externalLedPin, HIGH);
  } else {
    digitalWrite(externalLedPin, LOW);
  }
}
