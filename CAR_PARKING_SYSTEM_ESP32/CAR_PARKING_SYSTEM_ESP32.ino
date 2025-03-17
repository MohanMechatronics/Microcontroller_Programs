#include <ESP32Servo.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

int POSITION = 0;

int SLOT = 0;
int FLAG_1 = 0;
int FLAG_2 = 0;

LiquidCrystal_I2C lcd_1(0x27, 16, 2); // Address changed for ESP32 compatibility
Servo servo_4;

void setup() {
  lcd_1.init();
  pinMode(14, INPUT); // GPIO 14 for input
  servo_4.attach(13); // Pin 12 on ESP32
  pinMode(27, INPUT); // GPIO 27 for input

  SLOT = 3;
  servo_4.write(0); // Move servo to 0 degrees
  lcd_1.clear();
  lcd_1.backlight();
  lcd_1.setCursor(0, 0);
  lcd_1.print("      CAR");
  lcd_1.setCursor(0, 1);
  lcd_1.print(" PARKING SYSTEM");
  delay(1000);
  lcd_1.clear();
}

void loop()
{
  if (digitalRead(14) == 0 && FLAG_1 == 0) {
    if (SLOT > 0) {
      FLAG_1 = 1;
      if (FLAG_2 == 0) {
        for (POSITION = 0; POSITION <= 100; POSITION += 1) {
          servo_4.write(POSITION);
          delay(20); // Wait for 20 millisecond(s)
        }
        SLOT = (SLOT - 1);
      }
    } else {
      lcd_1.clear();
      lcd_1.setCursor(0, 0);
      lcd_1.print("     SORRY");
      lcd_1.setCursor(0, 1);
      lcd_1.print("  PARKING FULL");
      delay(3000); // Wait for 3000 millisecond(s)
      lcd_1.clear();
    }
  }

  if (digitalRead(27) == 0 && FLAG_2 == 0) {
    FLAG_2 = 1;
    if (FLAG_1 == 0) {
      for (POSITION = 0; POSITION <= 100; POSITION += 1) {
        servo_4.write(POSITION);
        delay(20); // Wait for 20 millisecond(s)
      }
      SLOT = (SLOT + 1);
    }
  }

  if (FLAG_1 == 1 && FLAG_2 == 1) {
    delay(1000); // Wait for 1000 millisecond(s)
    for (POSITION = 100; POSITION >= 1; POSITION -= 1) {
      servo_4.write(POSITION);
      delay(20); // Wait for 20 millisecond(s)
    }
    FLAG_1 = 0;
    FLAG_2 = 0;
  }

  lcd_1.setCursor(0, 0);
  lcd_1.print("    WELCOME!");
  lcd_1.setCursor(0, 1);
  lcd_1.print("SLOT LEFT :  ");
  lcd_1.print(SLOT);
}