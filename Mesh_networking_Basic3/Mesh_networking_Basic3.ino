#include <painlessMesh.h>
#include <ArduinoJson.h>

// mesh credentials has to be same for all!
#define MESH_PREFIX "AQUABOTICS"
#define pass "Caretta2"
#define port 5555

#define LED_Blue    2

String LEDState;
int adcReading;

Scheduler myScheduler;
painlessMesh mesh;

void sendCallback();
Task sendTask(TASK_SECOND * 1, TASK_FOREVER, &sendCallback);  


void sendCallback(){
  Serial.println();
  Serial.println("Start Sending ...");
  LEDState = "Bro";
  Serial.print("Value = "); Serial.println(LEDState);
  /*
   * { // Json format
   *  "ADCValue": adcReading
   * }
   */
   String msg;
   DynamicJsonDocument doc(1024);
   doc["Value"] = LEDState;
   serializeJson(doc, msg);
   mesh.sendBroadcast(msg);
   Serial.println("Message Sent");
}

void receivedCallback(uint32_t from, String msg){
  Serial.println();
  Serial.print("Message = "); Serial.println(msg);
  String json = msg.c_str();
  DynamicJsonDocument doc(1024);
  DeserializationError error = deserializeJson(doc, json);
  if(error){
    Serial.print("Failed Deserializing ... ");
    Serial.println(error.c_str());
  }
  adcReading = doc["ADCValue"];
  if(adcReading == 0){
    digitalWrite(LED_Blue, HIGH);
  }
  else{
    digitalWrite(LED_Blue, LOW);
  }
}

void setup() {
  Serial.begin(115200);
  pinMode(LED_Blue, OUTPUT);

  mesh.setDebugMsgTypes(ERROR | STARTUP); /*COMMUNICATION | CONNECTION);*/

  mesh.init(MESH_PREFIX, pass, &myScheduler, port); 
  mesh.onReceive(&receivedCallback);
  myScheduler.addTask(sendTask);
  sendTask.enable();
}

void loop() {
  mesh.update();
}