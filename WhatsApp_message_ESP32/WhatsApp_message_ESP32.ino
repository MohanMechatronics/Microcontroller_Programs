#include <WiFi.h> 
#include <ThingESP.h>

const int touchPin = 4; 

// change with your threshold value
const int threshold = 40;
// variable for storing the touch pin value 
int touchValue;
    
ThingESP32 thing("Mohankanth", "miracle", "hello");

void setup() {
  Serial.begin(115200);
  thing.SetWiFi("Iam Disguised", "gunasree");
  thing.initDevice();
}

String HandleResponse(String query) {
  touchValue = touchRead(touchPin);
  Serial.print(touchValue);
  String humid = "Dear Members,\nURGENT ALERT: Reservoir water level is currently at 85%, posing a risk of flooding. Please take immediate precautions. The elevated water level may lead to overflow, increasing the risk of flooding in low-lying areas.\nPrecautionary Measures:\n1. Stay Informed: Keep an eye on local news and official updates for real-time information.\n2. Evacuation Readiness: Residents in flood-prone areas, be prepared to evacuate as necessary. Follow instructions from local authorities.\n3. Emergency Contacts: Save and have readily available the emergency contact number: [123456789].\n";

  String temp = "Temperature: ";
  String alert = "ALERT";

  if (query == "temperature") {
    return temp;
  }
  else if (query == "humidity") {
    return humid;
  }
  else if (touchValue < threshold) {
    return alert;
  }
  else {
    return "Your query was invalid..";
  }
}

void loop() {
  thing.Handle();
}