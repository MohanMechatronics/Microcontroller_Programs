#include <WiFi.h>
#include <HTTPClient.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

#define OLED_SDA 21 // Pin connected to the SDA pin of OLED
#define OLED_SCL 22 // Pin connected to the SCL pin of OLED

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_SDA, OLED_SCL);


const int touchPin = 4;

// change with your threshold value
const int threshold = 40;
// variable for storing the touch pin value 
int touchValue;

const char* ssid = "Airtel_Disguised";
const char* password = "gunasree";
const char* apiKey = "gij2p1rtupfack5ck2itpfthembtxmvo";
const char* AppId = "818c3740-d17f-11e5-ae2d-0ea1b0410657";
const char* destinationPhoneNumber = "917358474625";



void setup() {
  Serial.begin(115200);

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

  // Connect to Wi-Fi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi");
  
  
}
void alert(){
  display.setTextSize(3);      // Set text size
  display.setTextColor(SSD1306_WHITE);  // Set text color to white
  display.setCursor(19, 28);   // Set cursor position (X, Y)
  display.print(F("ALERT!"));  // Print the alert message

  display.display();   // Display the content on the screen
  delay(5000);         // Wait for 5 seconds
  display.clearDisplay();   // Clear the display
  delay(1000);   
}

void loop() {
  touchValue = touchRead(touchPin);
  Serial.println(touchValue);
  if (touchValue<=threshold) {
    sendWhatsAppMessage("There seem to be a Leakage detected in sector - C in Metro pipe line");
    alert();
  } 
  else{
    display.display();
    display.clearDisplay();
  }
  delay(1000);
}

void sendWhatsAppMessage(String message) {
  HTTPClient http;

  String url = "https://botplatform.gupshup.io/bot/message/v3/" + String(AppId) + "/msg";
  String payload = "route=whatsapp&source=WhatsApp&destination=" + String(destinationPhoneNumber) + "&message=" + message;

  http.begin(url);
  http.addHeader("Content-Type", "application/x-www-form-urlencoded");
  http.addHeader("apikey", apiKey);

  int httpResponseCode = http.POST(payload);

  if (httpResponseCode > 0) {
    String response = http.getString();
    Serial.println(httpResponseCode);
    Serial.println(response);
  } else {
    Serial.print("Error on sending message. Response code: ");
    Serial.println(httpResponseCode);
  }

  http.end();
}