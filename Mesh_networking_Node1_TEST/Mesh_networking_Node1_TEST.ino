#include <painlessMesh.h>
#include "IPAddress.h"
#include <ArduinoJson.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>

// mesh credentials has to be same for all!
#define MESH_PREFIX "SWARMROBOTICS"
#define pass "Mechatronics"
#define port 5555

const int IR_Sensor = 4;
int Reading;

volatile uint64_t count = 0;
volatile uint64_t currentCount = -1;

unsigned long currentTime = 0;
unsigned long lastReadTime = 0;
unsigned int intervalDelay = 1000;

Scheduler myScheduler;
painlessMesh mesh;

#define UP 1
#define DOWN 2
#define TURN_LEFT 3
#define TURN_RIGHT 4
#define STOP 0

#define LEFT_MOTOR 0
#define RIGHT_MOTOR 1

#define FORWARD 1
#define BACKWARD -1

struct MOTOR_PINS
{
  int pinIN1;
  int pinIN2;    
};

std::vector<MOTOR_PINS> motorPins = 
{
  {12, 13},  //LEFT_MOTOR
  {15, 14},  //RIGHT_MOTOR 
};

void rotateMotor(int motorNumber, int motorDirection)
{
  if (motorDirection == FORWARD)
  {
    digitalWrite(motorPins[motorNumber].pinIN1, HIGH);
    digitalWrite(motorPins[motorNumber].pinIN2, LOW);    
  }
  else if (motorDirection == BACKWARD)
  {
    digitalWrite(motorPins[motorNumber].pinIN1, LOW);
    digitalWrite(motorPins[motorNumber].pinIN2, HIGH);     
  }
  else
  {
    digitalWrite(motorPins[motorNumber].pinIN1, LOW);
    digitalWrite(motorPins[motorNumber].pinIN2, LOW);       
  }
}

void sendCallback();
Task sendTask(TASK_SECOND * 1, TASK_FOREVER, &sendCallback);  


void sendCallback(){
  Serial.println();
  Serial.println("Start Sending ...");
  int Info = 5;
  Serial.print("Message = "); Serial.println(Info);
  /*
   * { // Json format
   *  "ADCValue": adcReading
   * }
   */
   String msg;
   DynamicJsonDocument doc(1024);
   doc["info"] = Info;
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
  Reading = doc["ADCValue"];
  switch(Reading)
  {

    case UP:
      rotateMotor(RIGHT_MOTOR, FORWARD);
      rotateMotor(LEFT_MOTOR, FORWARD);                 
      break;
  
    case DOWN:
      rotateMotor(RIGHT_MOTOR, BACKWARD);
      rotateMotor(LEFT_MOTOR, BACKWARD);  
      break;
  
    case TURN_LEFT:
      rotateMotor(LEFT_MOTOR, FORWARD); 
      break;
  
    case TURN_RIGHT:
      rotateMotor(RIGHT_MOTOR, FORWARD);   
      break;
  
    case STOP:
     rotateMotor(RIGHT_MOTOR, STOP);
      rotateMotor(LEFT_MOTOR, STOP);    
      break;
  
    default:
      rotateMotor(RIGHT_MOTOR, STOP);
      rotateMotor(LEFT_MOTOR, STOP);     
      break;
  }
}

void setUpPinModes()
{
  for (int i = 0; i < motorPins.size(); i++)
  {
    pinMode(motorPins[i].pinIN1, OUTPUT);
    pinMode(motorPins[i].pinIN2, OUTPUT);  
    rotateMotor(i, STOP);  
  }
}

void IRAM_ATTR isr()
{
  currentTime = millis();
  // IR Sensor is noisy so we add a debounce mechanism here
  if (currentTime - lastReadTime > intervalDelay)
  {
    count++;
    lastReadTime = currentTime;
  }
}

void setup() {
  setUpPinModes();
  Serial.begin(115200);
  pinMode(IR_Sensor, INPUT);
  attachInterrupt(digitalPinToInterrupt(IR_Sensor), isr, FALLING);

  mesh.setDebugMsgTypes( ERROR | STARTUP );

  mesh.init(MESH_PREFIX, pass, &myScheduler, port); 
  mesh.onReceive(&receivedCallback);
  myScheduler.addTask(sendTask);
  sendTask.enable();
}

void loop() {
  mesh.update();
  Serial.print("Count = ");
  Serial.println(count);
  if (currentCount != count)
  {
    currentCount = count;
    char buffer[50];
    ltoa(count, buffer, 10);
  }
  if ( count == 10 ){
    Serial.println("Good");
  }
  else{
  }
}