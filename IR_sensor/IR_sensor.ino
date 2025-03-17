#define IR_SENSOR    4
#define LED_red    2


void setup() {
  // initialize digital pin IR_SENSOR and LED_red as an output.
  Serial.begin(115200);  // initialize serial monitor
  pinMode(IR_SENSOR, INPUT);
  pinMode(LED_red, OUTPUT);
}

// the loop function runs over and over again forever
void loop() {
  if(digitalRead(4)==1){
    digitalWrite(LED_red, LOW);
    Serial.println("turn OFF");

  }
  if(digitalRead(4)==0){
    digitalWrite(LED_red,HIGH);
    Serial.println("turn on");
  }
}
