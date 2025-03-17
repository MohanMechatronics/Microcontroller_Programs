#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include "HX711.h"

// HX711 Pins
#define DOUT  25
#define CLK   26
HX711 scale;

int rbutton = 18; // Reset button to reset the scale to 0
String myString;
String cmessage; // Complete message
char buff[10];
float weight;
float calibration_factor = 206140; // Adjust this value based on your setup

// Initialize the LCD
LiquidCrystal_I2C lcd(0x27, 20, 4); // Adjust the I2C address if needed

void setup() {
  Serial.begin(9600);

  pinMode(rbutton, INPUT_PULLUP);

  // Initialize HX711
  scale.begin(DOUT, CLK); // Set HX711 pins
  scale.set_scale();
  scale.tare(); // Reset the scale to 0
  long zero_factor = scale.read_average(); // Get a baseline reading

  // Initialize the LCD
  lcd.init();
  lcd.backlight();
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Initializing...");
  delay(2000);
}

void loop() {
  scale.set_scale(calibration_factor); // Adjust to the calibration factor

  // Get the weight
  weight = scale.get_units(5); // Average of 5 readings
  myString = dtostrf(weight, 3, 3, buff);
  cmessage = "Weight: " + myString + " Kg";
  Serial.println(cmessage);
  cmessage = "";

  // Display on LCD
  lcd.clear();
  lcd.setCursor(0, 0); // Column, Row
  lcd.print("Weight:");
  lcd.setCursor(8, 0);
  lcd.print(myString);
  lcd.print(" Kg");

  // Reset scale if the button is pressed
  if (digitalRead(rbutton) == LOW) {
    lcd.setCursor(0, 2);
    lcd.print("Taring...");
    scale.set_scale();
    scale.tare(); // Reset the scale to 0
    delay(2000);
    lcd.setCursor(0, 2);
    lcd.print("Ready       ");
  }

  delay(500); // Adjust for your preference
}
