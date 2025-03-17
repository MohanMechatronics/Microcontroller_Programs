void setup() {
  pinMode(0, INPUT);
  pinMode(2, OUTPUT);

}

void loop() {
  int BUTTON = digitalRead(0);
  if(BUTTON == HIGH){
    digitalWrite(2, LOW);
  }
  else{
    digitalWrite(2, HIGH);
  }
}
