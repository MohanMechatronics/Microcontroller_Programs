void setup()
{
  pinMode(D7, INPUT);
  pinMode(D2, OUTPUT);
}

void loop()
{
  if (digitalRead(D7) == 0) {
    digitalWrite(D2, HIGH);
  } else {
    digitalWrite(D7, LOW);
  }
  delay(10); // Delay a little bit to improve simulation performance
}
