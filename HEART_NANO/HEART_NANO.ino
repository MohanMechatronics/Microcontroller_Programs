#include <Wire.h>
#include <PulseSensorPlayground.h>
#include <LCD_I2C.h>

LCD_I2C lcd(0x27, 16, 2);// I2C address of the LCD (0x27 is common)

// Pulse Sensor settings
const int pulsePin = A0;    // Pulse Sensor connected to analog pin A0
const int threshold = 550;  // Adjust this value to match the sensor sensitivity

// Create an instance of the PulseSensorPlayground object
PulseSensorPlayground pulseSensor;

void setup() {
  // Initialize serial communication for debugging (optional)
  Serial.begin(9600);

  // Initialize the LCD
  lcd.begin();
  lcd.backlight();

  // Display initial message
  lcd.setCursor(0, 0);
  lcd.print("Heart Rate Monitor");
  delay(2000);
  lcd.clear();

  // Set up the PulseSensor object
  pulseSensor.analogInput(pulsePin);
  pulseSensor.setThreshold(threshold);

  // Start reading pulses
  if (pulseSensor.begin()) {
    lcd.setCursor(0, 0);
    lcd.print("Pulse Sensor Ready");
  } else {
    lcd.setCursor(0, 0);
    lcd.print("Sensor Failed");
  }
}

void loop() {
  // Get heart rate (BPM) if a beat is detected
  int myBPM = pulseSensor.getBeatsPerMinute();

  // If a heartbeat is detected and BPM is valid
  if (pulseSensor.sawStartOfBeat()) {
    Serial.println("Heartbeat detected");
    Serial.print("BPM: ");
    Serial.println(myBPM);

    // Display the heart rate on the LCD
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Heart Rate: ");
    lcd.setCursor(0, 1);
    lcd.print(myBPM);
    lcd.print(" BPM");
  }

  // Optional: Small delay between sensor reads
  delay(1000);
}
