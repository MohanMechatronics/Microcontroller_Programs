/* Comment this out to disable prints and save space */
#define BLYNK_PRINT Serial

/* Fill-in your Template ID (only if using Blynk.Cloud) */
#define BLYNK_TEMPLATE_ID "TMPL3yQnxRl8n"
#define BLYNK_TEMPLATE_NAME "Mohankanth"
#define BLYNK_AUTH_TOKEN "bdIs8GsCeszQmDUXyzDnmsocbgoQ9gSP"

// mohankanth12345@gnail.com

// #define BLYNK_TEMPLATE_ID "TMPLhIWab7YJ"
// #define BLYNK_TEMPLATE_NAME "Multi Automation"
// #define BLYNK_AUTH_TOKEN "kzTUYO_UMDiYZyXxisW8nX-g1LJWGFs8"

// mohankanofficial@gmail.com

#include "esp_wifi.h"
#include <WiFi.h>
#include <BlynkSimpleEsp32.h>

// You should get Auth Token in the Blynk App.
// Go to the Project Settings (nut icon).
char auth[] = BLYNK_AUTH_TOKEN;

// Your WiFi credentials.
// Set password to "" for open networks.
const char* ssid = "Iam Disguised";
const char* password = "gunasree";

BLYNK_WRITE(V0)
{
  int value1 = param.asInt();
  Serial.println(value1);
} 

BLYNK_WRITE(V1)
{
  int value2 = param.asInt();
  Serial.println(value2);
} 

void setup()
{
  // Debug console
  Serial.begin(115200);

  Blynk.begin(auth, ssid, password);
  // You can also specify server:
  //Blynk.begin(auth, ssid, pass, "blynk-cloud.com", 80);
  //Blynk.begin(auth, ssid, pass, IPAddress(192,168,1,100), 8080);
}

void loop()
{
  Blynk.run();
}
