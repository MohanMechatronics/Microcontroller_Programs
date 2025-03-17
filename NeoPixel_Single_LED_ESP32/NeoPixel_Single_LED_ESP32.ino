#include <Adafruit_NeoPixel.h>

#define LED_PIN     13

// How many NeoPixels are attached to the microcontroller?
#define LED_COUNT  1

// NeoPixel brightness, 0 (min) to 255 (max)
#define BRIGHTNESS 150 // Set BRIGHTNESS to about 1/5 (max = 255)

// Declare our NeoPixel strip object:
Adafruit_NeoPixel strip(LED_COUNT, LED_PIN, NEO_GRB + NEO_KHZ800);

void setup() {
  strip.begin();           // INITIALIZE NeoPixel strip object (REQUIRED)
  strip.show();            // Turn OFF all pixels ASAP
  strip.setBrightness(BRIGHTNESS);
}

void loop() {
  // Fill along the length of the strip in various colors...
  colorWipe(strip.Color(255,   0,   0)     , 50); // Red
  delay(2000);
  colorWipe(strip.Color(  0, 255,   0)     , 50); // Green
  delay(2000);
  colorWipe(strip.Color(  0,   0, 255)     , 50); // Blue
  delay(2000);
  colorWipe(strip.Color( 255, 255,  0)     , 50);  // Yellow
  delay(2000);
  colorWipe(strip.Color( 255,  0, 255)     , 50);  // Magenta
  delay(2000);
  colorWipe(strip.Color(  0, 255, 255)     , 50);  // Cyan
  delay(2000);
}


void colorWipe(uint32_t color, int wait) {
  for(int i=0; i<strip.numPixels(); i++) { // For each pixel in strip...
    strip.setPixelColor(i, color);         //  Set pixel's color (in RAM)
    strip.show();                          //  Update strip to match
    delay(wait);                           //  Pause for a moment
  }
}
