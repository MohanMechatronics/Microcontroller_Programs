/*const int ledPin = 12;  // 12 corresponds to GPIO12

// setting PWM properties
const int freq = 1000;
const int ledChannel = 4;
const int resolution = 8;
 */
const int ledPin = 12;

void setup(){
  // configure LED PWM functionalitites
  ledcSetup(4,2000,8); //ledcSetup(ledChannel, freq - 2Khz, resolution);
  
  // attach the channel to the GPIO to be controlled
  ledcAttachPin(12, 4); //ledcAttachPin(ledPin, ledChannel);
}
 
void loop(){
  // increase the LED brightness
  for(int dutyCycle = 0; dutyCycle <= 255; dutyCycle++){   
    // changing the LED brightness with PWM
    ledcWrite(4, dutyCycle); //ledcWrite(ledChannel, dutyCycle);
    delay(15);
  }

  // decrease the LED brightness
  for(int dutyCycle = 255; dutyCycle >= 0; dutyCycle--){
    // changing the LED brightness with PWM
    ledcWrite(4, dutyCycle);  //ledcWrite(ledChannel, dutyCycle); 
    delay(15);
  }
}
