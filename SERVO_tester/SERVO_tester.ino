static const int potentiometerPin = 12;// pin printed as G12 or 12

void setup() {
    Serial.begin(115200);
}

void loop() {
    int servoPosition = analogRead(potentiometerPin);
    Serial.println(servoPosition);
    delay(500);
}