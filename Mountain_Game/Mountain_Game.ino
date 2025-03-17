#include <FastLED.h>

#define LED_PIN 4
#define NUM_LEDS 100

#define RED_BUTTON_PIN 2
#define GREEN_BUTTON_PIN 3

CRGB leds[NUM_LEDS];

int redPos = 0;       // Start position for Red Player
int greenPos = 99;    // Start position for Green Player
bool gameOver = false;

void setup() {
    FastLED.addLeds<WS2812, LED_PIN, GRB>(leds, NUM_LEDS);
    pinMode(RED_BUTTON_PIN, INPUT_PULLUP);
    pinMode(GREEN_BUTTON_PIN, INPUT_PULLUP);
    resetGame();
}

void loop() {
    if (!gameOver) {
        if (digitalRead(RED_BUTTON_PIN) == LOW) {  // Red Player Button Pressed
            delay(150); // Simple debounce
            if (redPos < 50) {
                redPos++;
                updateLeds();
            }
            checkWinner();
        }
        
        if (digitalRead(GREEN_BUTTON_PIN) == LOW) { // Green Player Button Pressed
            delay(150); // Simple debounce
            if (greenPos > 49) {
                greenPos--;
                updateLeds();
            }
            checkWinner();
        }
    }
}

void updateLeds() {
    fill_solid(leds, NUM_LEDS, CRGB::Black); // Clear all LEDs
    leds[redPos] = CRGB::Red;    // Set Red Player LED
    leds[greenPos] = CRGB::Green; // Set Green Player LED
    FastLED.show();
}

void checkWinner() {
    if (redPos == 50) {
        blinkWinner(CRGB::Red);
    } else if (greenPos == 49) {
        blinkWinner(CRGB::Green);
    }
}

void blinkWinner(CRGB winnerColor) {
    gameOver = true;
    for (int i = 0; i < 3; i++) {  // Blink the winner's color 3 times
        fill_solid(leds, NUM_LEDS, winnerColor);
        FastLED.show();
        delay(500);
        fill_solid(leds, NUM_LEDS, CRGB::Black);
        FastLED.show();
        delay(500);
    }
    delay(1000);
    resetGame();
}

void resetGame() {
    redPos = 0;
    greenPos = 99;
    gameOver = false;
    updateLeds();
}
