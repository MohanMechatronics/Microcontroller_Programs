const int IR_Sensor = 4;
#define LED_red    2

volatile uint64_t count = 0;
volatile uint64_t currentCount = -1;

unsigned long currentTime = 0;
unsigned long lastReadTime = 0;
unsigned int intervalDelay = 1000;

void IRAM_ATTR isr()
{
  currentTime = millis();
  // IR Sensor is noisy so we add a debounce mechanism here
  if (currentTime - lastReadTime > intervalDelay)
  {
    count++;
    lastReadTime = currentTime;
  }
}

void setup()
{
  Serial.begin(115200);
  pinMode(IR_Sensor, INPUT);
  pinMode(LED_red, OUTPUT);
  attachInterrupt(digitalPinToInterrupt(IR_Sensor), isr, FALLING);

}
void loop()
{
  Serial.print("Count = ");
  Serial.println(count);
  if (currentCount != count)
  {
    currentCount = count;
    char buffer[50];
    ltoa(count, buffer, 10);
  }
  if ( count == 10 ){
    digitalWrite(LED_red,HIGH);
  }
  else{
    digitalWrite(LED_red, LOW);
  }
}