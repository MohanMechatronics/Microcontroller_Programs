#include <Wire.h>          // Include I2C library for communication with LCD
#include <LiquidCrystal_I2C.h>  // Include the LCD library

// Initialize the LCD with I2C address (0x27 or 0x3F based on your module)
LiquidCrystal_I2C lcd(0x27, 16, 2);  

int sensorPin = A0;  // MQ sensor analog output pin connected to A0

void setup() {
  lcd.begin();                // Initialize the LCD
  lcd.backlight();            // Turn on the backlight
  Serial.begin(9600);         // Start serial communication

  lcd.setCursor(0, 0);        // Set cursor to the first row
  lcd.print("Gas Detection");
  delay(2000);                // Display startup message for 2 seconds
  lcd.clear();                // Clear the LCD for new display
}

void loop() {
  int sensorValue = analogRead(sensorPin);  // Read the analog value from sensor
  int gasLevel = map(sensorValue, 0, 1023, 0, 100);  // Scale to percentage

  // Display the gas level on the LCD
  lcd.setCursor(0, 0);  // First row
  lcd.print("Gas Level: ");  // Print label
  lcd.setCursor(11, 0);
  lcd.print(gasLevel);   // Print gas level percentage
  lcd.print("%");

  // Optional: Print sensor value to Serial Monitor
  Serial.print("Gas Sensor Value: ");
  Serial.println(sensorValue);
  
  delay(1000);  // Wait for 1 second before updating the value
}