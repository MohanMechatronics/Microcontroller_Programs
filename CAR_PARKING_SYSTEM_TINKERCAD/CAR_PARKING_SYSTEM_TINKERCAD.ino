// C++ code
//
#include <Servo.h>

#include <LiquidCrystal_I2C.h>

int POSITION = 0;

int i = 0;

int j = 0;

int SLOT = 0;

int FLAG_1 = 0;

int FLAG_2 = 0;

Servo servo_4;

LiquidCrystal_I2C lcd_1(39, 16, 2);

void setup()
{
  servo_4.attach(4, 500, 2500);
  lcd_1.init();
  pinMode(3, INPUT);
  pinMode(5, INPUT);

  SLOT = 3;
  servo_4.write(0);
  lcd_1.clear();
  lcd_1.backlight();
  lcd_1.setCursor(0, 0);
  lcd_1.print("      CAR");
  lcd_1.setCursor(0, 1);
  lcd_1.print(" PARKING SYSTEM");
  delay(2000); // Wait for 2000 millisecond(s)
  lcd_1.clear();
}

void loop()
{
  if (digitalRead(3) == 0 && FLAG_1 == 0) {
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
  if (digitalRead(5) == 0 && FLAG_2 == 0) {
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
