#include "FS.h"
#include <SPI.h>
#include <TFT_eSPI.h>      
TFT_eSPI tft = TFT_eSPI(); 
#define CALIBRATION_FILE "/TouchCalData1"
#define REPEAT_CAL false
#define LABEL1_FONT &FreeSansOblique12pt7b // Key label font 1
#define LABEL2_FONT &FreeSansBold12pt7b    // Key label font 2
TFT_eSPI_Button ON;
TFT_eSPI_Button OFF;
#define LED 12

void setup() 
{
  touch_calibrate();
  Serial.begin(115200);
  tft.init();
  pinMode(LED, OUTPUT);
  pinMode(0, INPUT_PULLDOWN); 
  digitalWrite(LED, LOW);
  tft.setRotation(2);
    
  tft.fillScreen(TFT_BLACK);
  tft.setFreeFont(LABEL2_FONT);
  ON.initButton(&tft, 60, 200, 100, 60, TFT_WHITE, TFT_GREEN, TFT_BLACK, "ON", 1);
  ON.drawButton();
  OFF.initButton(&tft, 180, 200, 100, 60, TFT_WHITE, TFT_RED, TFT_WHITE, "OFF", 1);
  OFF.drawButton();
  tft.fillCircle(120, 100, 30, TFT_RED);
  tft.setCursor(0, 25);
  tft.setTextSize(1);
  tft.setTextColor(TFT_YELLOW); 
  tft.print("TeachMe Something");
}

void loop() 
{
  static bool recalibrating = false;

  // Check if pin 0 is pressed
  if (digitalRead(0) == LOW && !recalibrating) {
    recalibrating = true; // Avoid repeated recalibration due to debounce
    touch_calibrate();
    recalibrating = false;
  }

  uint16_t t_x = 0, t_y = 0; 
  bool pressed = tft.getTouch(&t_x, &t_y);
  
  if (pressed && ON.contains(t_x, t_y))   ON.press(true); 
  else ON.press(false);  
  if (pressed && OFF.contains(t_x, t_y))   OFF.press(true); 
  else OFF.press(false);  
  if(!pressed)
    return;

  if (ON.justPressed()) 
  {
    ON.drawButton(true); 
    tft.fillCircle(120, 100, 30, TFT_GREEN);
    Serial.println("ON Button Pressed");
    digitalWrite(LED, HIGH);
    delay(100);
    ON.drawButton();
  }

  if (OFF.justPressed()) 
  {
    OFF.drawButton(true); 
    tft.fillCircle(120, 100, 30, TFT_RED);
    Serial.println("OFF Button Pressed");
    digitalWrite(LED, LOW);
    delay(100);
    OFF.drawButton(); 
  }
}

void touch_calibrate()
{
  uint16_t calData[5];
  uint8_t calDataOK = 0;

  // check file system exists
  if (!SPIFFS.begin()) {
    Serial.println("formatting file system");
    SPIFFS.format();
    SPIFFS.begin();
  }

  // check if calibration file exists and size is correct
  if (SPIFFS.exists(CALIBRATION_FILE)) {
    if (REPEAT_CAL)
    {
      // Delete if we want to re-calibrate
      SPIFFS.remove(CALIBRATION_FILE);
    }
    else
    {
      File f = SPIFFS.open(CALIBRATION_FILE, "r");
      if (f) {
        if (f.readBytes((char *)calData, 14) == 14)
          calDataOK = 1;
        f.close();
      }
    }
  }

  if (calDataOK && !REPEAT_CAL) {
    // calibration data valid
    tft.setTouch(calData);
  } else {
    // data not valid so recalibrate
    tft.fillScreen(TFT_BLACK);
    tft.setCursor(20, 0);
    tft.setTextFont(2);
    tft.setTextSize(1);
    tft.setTextColor(TFT_WHITE, TFT_BLACK);

    tft.println("Touch corners as indicated");

    tft.setTextFont(1);
    tft.println();

    if (REPEAT_CAL) {
      tft.setTextColor(TFT_RED, TFT_BLACK);
      tft.println("Set REPEAT_CAL to false to stop this running again!");
    }

    tft.calibrateTouch(calData, TFT_MAGENTA, TFT_BLACK, 15);

    tft.setTextColor(TFT_GREEN, TFT_BLACK);
    tft.println("Calibration complete!");

    // store data
    File f = SPIFFS.open(CALIBRATION_FILE, "w");
    if (f) {
      f.write((const unsigned char *)calData, 14);
      f.close();
    }
  }
}

//------------------------------------------------------------------------------------------