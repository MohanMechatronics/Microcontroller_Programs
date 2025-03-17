// Created by Simple Circuits 

#include <Wire.h> 
#include <LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd(0x27,16,2);   
#include <Servo.h> 
int POSITION = 0;
Servo servo_4;
int IR1 = 3;
int IR2 = 5;
int Slot = 4;           //Total number of parking Slots
int flag1 = 0;
int flag2 = 0;

void setup() {
  Serial.begin(9600); 
    lcd.init(); //initialize the lcd
    lcd.backlight(); //open the backlight
pinMode(IR1, INPUT);
pinMode(IR2, INPUT);
servo_4.attach(4, 500, 2500);
servo_4.write(0);

lcd.setCursor (0,0);
lcd.print("     CAR    ");
lcd.setCursor (0,1);
lcd.print(" PARKING SYSTEM ");
delay (2000);
lcd.clear();  
}



void loop(){ 
if(digitalRead (IR1) == LOW && flag1==0){

if(Slot>0){flag1=1;

    if(flag2==0){for (POSITION = 0; POSITION <= 100; POSITION += 1) {
      servo_4.write(POSITION);
      delay(20);
      }
      Slot = Slot-1;
      }

}else{

lcd.setCursor (0,0);
lcd.print("    SORRY :(    ");  
lcd.setCursor (0,1);
lcd.print("  Parking Full  "); 
delay (3000);
lcd.clear(); 
 }
}

if(digitalRead (IR2) == LOW && flag2==0){flag2=1;

if(flag1==0){for (POSITION = 0; POSITION <= 100; POSITION += 1) {
      servo_4.write(POSITION);
      delay(20);
      }
      Slot = Slot+1;
  }
}

if(flag1==1 && flag2==1){
delay (1000);
for (POSITION = 100; POSITION >= 1; POSITION -= 1) {
      servo_4.write(POSITION);
      delay(20);
}
flag1=0, flag2=0;

}

lcd.setCursor (0,0);
lcd.print("    WELCOME!    ");
lcd.setCursor (0,1);
lcd.print("Slot Left: ");
lcd.print(Slot);
}


