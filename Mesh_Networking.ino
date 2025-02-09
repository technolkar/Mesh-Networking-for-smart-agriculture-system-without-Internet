//This code is written by Technolokar and it is used for making the 
//Mesh Networking for Agriculture system monitoring without Internet by using the Nodemcu.

#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "painlessMesh.h"
#include "DHT.h"
#include <ESP8266WiFi.h>
//#include <BlynkSimpleEsp8266.h>

#define DHTPIN 2
#define DHTTYPE DHT11
#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 32 // OLED display height, in pixels
#define mlevelIN A0
#define trigPin 12
#define switchPin 13
Adafruit_SSD1306 display = Adafruit_SSD1306(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire);
DHT dht(DHTPIN, DHTTYPE);

#define   MESH_PREFIX     "technolkar"
#define   MESH_PASSWORD   "abcd1234"
#define   MESH_PORT       5555

Scheduler userScheduler; 
painlessMesh  mesh;
void sendMessage() ; 
Task taskSendMessage( TASK_SECOND * 1 , TASK_FOREVER, &sendMessage );

void sendMessage()
{
  Serial.println();
  Serial.println("Start Sending....");
  
  DynamicJsonDocument doc(1024);
  float h = dht.readHumidity();
  float t = dht.readTemperature();
  int m = analogRead(mlevelIN);
  int motorState = analogRead(trigPin);
  if(m<=1000)
    doc["MOISTURE"] = "Wet";
  else 
    doc["MOISTURE"] = "Dry";
  if(!motorState)
    doc["MOTOR STATE"] = "OFF";
  else
    doc["MOTOR STATE"] = "ON";      
  doc["TEMPERATURE"] = t;
  doc["HUMIDITY"] = h;
  String msg;
  serializeJson(doc, msg);
  
  mesh.sendBroadcast( msg );
  taskSendMessage.setInterval((TASK_SECOND * 1, TASK_SECOND * 10));
}

// Needed for painless library
void receivedCallback( uint32_t from, String &msg ) {
  Serial.println();
  Serial.print("Message = ");Serial.println(msg);
  String json;
  DynamicJsonDocument doc(1024);
  json = msg.c_str();
  DeserializationError error = deserializeJson(doc, json);
  if (error)
  {
    Serial.print("deserializeJson() failed: ");
    Serial.println(error.c_str());
  }
}
void newConnectionCallback(uint32_t nodeId) {
  Serial.printf("--> startHere: New Connection, nodeId = %u\n", nodeId);
}
void changedConnectionCallback() {
  Serial.printf("Changed connections\n");
}
void nodeTimeAdjustedCallback(int32_t offset) {
  Serial.printf("Adjusted time %u. Offset = %d\n", mesh.getNodeTime(), offset);
}

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);

  mesh.setDebugMsgTypes( ERROR | STARTUP );  
  mesh.init( MESH_PREFIX, MESH_PASSWORD, &userScheduler, MESH_PORT );
  mesh.onReceive(&receivedCallback);
  mesh.onNewConnection(&newConnectionCallback);
  mesh.onChangedConnections(&changedConnectionCallback);
  mesh.onNodeTimeAdjusted(&nodeTimeAdjustedCallback);
  userScheduler.addTask( taskSendMessage );
  taskSendMessage.enable();

  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
    display.clearDisplay();
  Serial.begin(115200);
  //Blynk.begin(auth, MESH_PREFIX, MESH_PASSWORD);
  pinMode(mlevelIN,INPUT);
  pinMode(trigPin, OUTPUT);
  pinMode(switchPin, INPUT);
  dht.begin();
}

void loop() {
  //Blynk.run();
  float h = dht.readHumidity();
  float t = dht.readTemperature();
  float f = dht.readTemperature(true);
  int m = analogRead(mlevelIN);
  int trigValue = digitalRead(trigPin);
  int switchValue = digitalRead(switchPin);
  Serial.print("Switch Value:");
  Serial.println(switchValue);
  if(m<=1000)
  {
    displayOled(h,t,"Wet","OFF");
    delay(1000);
    digitalWrite(trigPin, LOW);
  }
  else
  {
    displayOled(h,t,"Dry","ON");
    digitalWrite(trigPin, HIGH);
  }  

  delay(1000);
  mesh.update();
}

void displayOled(float h,float t,char m[], char motor[]) {
  display.clearDisplay();
  display.setTextColor(WHITE);
  display.setTextSize(1);
  display.setCursor(0,0);
  display.print("Humidity:");
  display.setCursor(60,0);
  display.print(h);
  display.setCursor(0,8);
  display.print("Temperature:");
  display.setCursor(72,8);
  display.print(t);
  display.print((char)247); // degree symbol
  display.print("C");
  display.setCursor(0,16);
  display.print("Moisture Level:");
  display.print(m);
  display.setCursor(0,22);
  display.print("Pump State: ");
  display.print(motor);
  display.display();
}
