#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

#define OLED_SDA 21 // Pin connected to the SDA pin of OLED
#define OLED_SCL 22 // Pin connected to the SCL pin of OLED

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_SDA, OLED_SCL);


void setup() {

  Wire.begin(OLED_SDA, OLED_SCL);
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);

  display.clearDisplay();
  display.setCursor(0,0);
  display.setTextSize(1);
  display.setTextColor(WHITE);

  display.setCursor(1,28);
  display.setTextSize(3);
  display.print("RETHISH");
  delay(2500);
  display.display();
  display.clearDisplay();
}

void loop() {
Wire.begin(OLED_SDA, OLED_SCL);
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);

    display.setTextSize(3);      // Set text size
  display.setTextColor(SSD1306_WHITE);  // Set text color to white
  display.setCursor(19, 28);   // Set cursor position (X, Y)
  display.print(F("ALERT!"));  // Print the alert message

  display.display();   // Display the content on the screen
  delay(5000);         // Wait for 5 seconds
  display.clearDisplay();   // Clear the display
  delay(1000);         // Pause for 1 second before the next loop
}