#include "ArduinoJson.h"
#include "ESP8266WiFi.h"
#include "ESP8266HTTPClient.h"
#include "config.h"

// global declarations
const size_t bufferSize = JSON_OBJECT_SIZE(2);
DynamicJsonBuffer jsonBuffer(bufferSize);
JsonObject& root = jsonBuffer.createObject();
HTTPClient http;
volatile int messageSend = false;

// Door status
// 1 is open
// 0 is closed

// function to init Serial
void initSerial() {
  Serial.begin(115200);
  Serial.setTimeout(2000);
}

// function to init wifi and try to connect
void wifiConnect() {
  WiFi.mode(WIFI_STA);
  WiFi.begin(SSID, PASSWORD);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    printMessage(".");
  }
  printMessage("Connected, IP address: ");
  Serial.println(WiFi.localIP());
}

// functoin to create post payload
JsonObject& createPayload(String doorStatus) {
  root["status"] = doorStatus;
  // root.prettyPrintTo(Serial);
  return root;
}

// function to send the data to firebase
void sendData(int doorStatus) {
  String status = doorStatus == 1 ? "open" : "closed";
  JsonObject& data = createPayload(status);
  String postData = "";
  data.printTo(postData);
  http.begin(API_URL);
  http.addHeader("Content-Type", "application/json");
  http.POST(postData);
  printMessage(postData);  
  printMessage(http.getString());
  http.end();
}

void printMessage(String msg) {
  Serial.println(msg);
}

int createDelayInSeconds(int seconds) {
  return ( seconds * 1000 );
}

void setup(){
  initSerial();
  wifiConnect();
  pinMode(SENSOR_PIN, INPUT_PULLUP);
}

void loop() {
 int doorStatus = digitalRead(SENSOR_PIN);
 printMessage(String(doorStatus));
 printMessage(String(messageSend));
 if ((doorStatus == 1) && (messageSend == false)) {
  messageSend = true;
  printMessage("Door status sent");
  sendData(doorStatus);
 }

 if ((doorStatus == 0) && (messageSend == true)) {
  messageSend = false;
  printMessage("Door status sent");
  sendData(doorStatus);
 }

 delay(createDelayInSeconds(3));
}
