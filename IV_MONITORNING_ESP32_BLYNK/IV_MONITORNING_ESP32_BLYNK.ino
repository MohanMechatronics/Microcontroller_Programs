#include <WiFi.h>
#include <WiFiClient.h>
#include <BlynkSimpleEsp32.h>
#include <LiquidCrystal_I2C.h>
#include "HX711.h"

// Define HX711 Pins
#define DOUT  25
#define CLK   26
#define BUZZER 27

HX711 scale; // Use default constructor without arguments

// Blynk credentials
char auth[] = "z7s02bmdQXOc1Y7zy1RgkLCV-AEh_8NF";
char ssid[] = "premkumat";
char pass[] = "00000000";

// LCD initialization
LiquidCrystal_I2C lcd(0x27, 20, 4);

// Variables
int liter;
int val;
float weight;
float calibration_factor = 102500; // Adjust this value for your load cell sensor

void setup() {
  // Set up serial monitor
  Serial.begin(115200);

  // Initialize LCD
  lcd.init();
  lcd.backlight();
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Connecting to");
  lcd.setCursor(0, 1);
  lcd.print("Network...");

  // Initialize Buzzer
  pinMode(BUZZER, OUTPUT);

  // Initialize HX711
  scale.begin(DOUT, CLK);
  Serial.println("Remove all weight from scale");
  scale.set_scale();
  scale.tare(); // Reset the scale to 0
  long zero_factor = scale.read_average(); // Get a baseline reading
  Serial.print("Zero factor: ");
  Serial.println(zero_factor);

  // Connect to Blynk
  Blynk.begin(auth, ssid, pass);

  // Wait until connected
  while (!Blynk.connected()) {
    Serial.println("Attempting to connect to Blynk...");
    lcd.setCursor(0, 2);
    lcd.print("Trying...");
    delay(1000);
  }

  // Indicate connection success
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Connected to WiFi");
  delay(2000); // Show message for 2 seconds
  lcd.clear();
}

void loop() {
  Blynk.run();
  measureWeight();
}

void measureWeight() {
  scale.set_scale(calibration_factor); // Adjust to this calibration factor
  weight = scale.get_units(5);
  if (weight < 0) {
    weight = 0.00;
  }

  liter = weight * 1000;
  val = liter;
  val = map(val, 0, 505, 0, 100);

  // Display data on LCD
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("IOT Based IVBag");
  lcd.setCursor(0, 1);
  lcd.print("    Monitoring");
  lcd.setCursor(0, 2);
  lcd.print("IV BO ");
  lcd.print(liter);
  lcd.print("mL");
  lcd.setCursor(0, 3);
  lcd.print("IV PER ");
  lcd.print(val);
  lcd.print("%");

  // Log to Serial Monitor
  Serial.print("Kilogram: ");
  Serial.print(weight);
  Serial.println(" Kg");
  Serial.print("IV Bottle: ");
  Serial.print(liter);
  Serial.println(" mL");
  Serial.print("IV Bag Percent: ");
  Serial.print(val);
  Serial.println("%");

  // Trigger alerts
  if (val <= 20 && val >= 11) {
    Blynk.logEvent("iv_alert", "IV Bottle is 50%");
    digitalWrite(BUZZER, HIGH);
    delay(50);
    digitalWrite(BUZZER, LOW);
    delay(50);
  } else if (val <= 10) {
    Blynk.logEvent("iv_alert", "IV Bottle is too LOW");
    digitalWrite(BUZZER, HIGH);
  } else {
    digitalWrite(BUZZER, LOW);
  }

  // Send data to Blynk
  Blynk.virtualWrite(V0, liter);
  Blynk.virtualWrite(V1, val);

  delay(500); // Update every 500ms
}
