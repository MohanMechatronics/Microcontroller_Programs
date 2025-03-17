#define trigPin D5
#define echoPin D6
#define LED_Green D0
 int duration;
 int distance;
void setup() {
  Serial.begin(115200);
  pinMode(trigPin, OUTPUT); 
  pinMode(echoPin, INPUT);
  pinMode(LED_Green, OUTPUT);
}
 void loop() {
  digitalWrite(trigPin, LOW); 
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH); 
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  duration = pulseIn(echoPin, HIGH);
  distance= duration*0.034/2;
  Serial.println(distance);
  delay(1000);
  if (distance<10){  
    digitalWrite(LED_Green, HIGH);
  }
  else{
    digitalWrite(LED_Green, LOW);
  }  
}  
