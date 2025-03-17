#define LED_Blue 2
#define LED_red 3


void setup() {
 // initialize digital pin LED_BUILTSIN as an output.
  Serial.begin(9600);
  pinMode(LED_Blue, OUTPUT);
  pinMode(LED_red, INPUT);
}

void loop()
{
  if (digitalRead(LED_red) == 0) {
    digitalWrite(LED_Blue, HIGH);
  } else {
    digitalWrite(LED_Blue, LOW);
  }
  delay(10); // Delay a little bit to improve simulation performance
}
