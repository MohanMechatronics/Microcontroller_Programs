#include <painlessMesh.h>
#include <ArduinoJson.h>

// mesh credentials has to be same for all!
#define id "AQUABOTICS"
#define pass "Caretta2"
#define port 5555


int adcReading;

Scheduler myScheduler;
painlessMesh mesh;

void sendCallback();
Task sendTask(TASK_SECOND * 1, TASK_FOREVER, &sendCallback);  


void sendCallback(){
  Serial.println();
  Serial.println("Start Sending ...");
  adcReading = 1;
  Serial.print("ADC Value = "); Serial.println(digitalRead(0));
  /*
   * { // Json format
   *  "ADCValue": adcReading
   * }
   */
   String msg;
   DynamicJsonDocument doc(1024);
   doc["ADCValue"] = digitalRead(0);
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
}

void setup() {
  Serial.begin(115200);

  mesh.setDebugMsgTypes(ERROR | STARTUP);

  mesh.init(id, pass, &myScheduler, port); 
  mesh.onReceive(&receivedCallback);
  myScheduler.addTask(sendTask);
  sendTask.enable();

  pinMode(0, INPUT);
  pinMode(2, OUTPUT);
}

void loop() {
  int BUTTON = digitalRead(0);
  if(BUTTON == HIGH){
    digitalWrite(2, LOW);
  }
  else{
    digitalWrite(2, HIGH);
  }
  mesh.update();
}