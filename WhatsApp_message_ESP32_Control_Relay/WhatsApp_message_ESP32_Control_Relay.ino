#include <WiFi.h>
#include <ThingESP.h>

ThingESP32 thing("Mohankanth", "miracle", "hello");

int LIGHT = 14;
int LIGHT2 = 27;


void setup()
{
  Serial.begin(115200);
  pinMode(LIGHT, OUTPUT);
  pinMode(LIGHT2, OUTPUT);
  digitalWrite(LIGHT, 1);
  digitalWrite(LIGHT2, 1);
  thing.SetWiFi("Iam Disguised", "gunasree");
  thing.initDevice();
}


String HandleResponse(String query)
{
  if (query == "light on") {
    digitalWrite(LIGHT, 0);
    Serial.println("light on");
    return "Done: Light Turned ON";
  }

  else if (query == "light off") {
    digitalWrite(LIGHT, 1);
    Serial.println("light off");
    return "Done: Light Turned OFF";
  }
    
    else if (query == "light2 on") {
    digitalWrite(LIGHT2, 0);
    Serial.println("light2 on");
    return "Done: Light2 Turned ON";
  }
    
    else if (query == "light2 off") {
    digitalWrite(LIGHT2, 1);
    Serial.println("light2 on");
    return "Done: Light2 Turned OFF";
  }
    else if (query == "light2 status")
    return digitalRead(LIGHT2) ? "LIGHT2 is ON" : "LIGHT2 is OFF" ;    

  else if (query == "light status")
    return digitalRead(LIGHT) ? "LIGHT is ON" : "LIGHT is OFF" ;
  else return "Your query was invalid..";

}


void loop()
{
  thing.Handle();

}